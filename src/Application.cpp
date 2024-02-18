#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Systems/InputSystem.hpp"
#include "Systems/TimeKeeper.hpp"
#include "Entity/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "OGL/Graphics.hpp"
#include "Systems/ResourceDatabase.hpp"
#include "Systems/RenderSystem.hpp"
#include "Resources/ModelLoader.hpp"
#include "Resources/ShaderLoader.hpp"
#include "Resources/Material.hpp"
#include "Resources/TextureLoader.hpp"
#include "Systems/LightingSystem.hpp"


std::unique_ptr<TimeKeeper> timeKeeper = std::make_unique<TimeKeeper>();
std::unique_ptr<InputSystem> inputSystem = std::make_unique<InputSystem>();
std::unique_ptr<RenderSystem> rendererSystem = std::make_unique<RenderSystem>();
std::unique_ptr<LightingSystem> lightingSystem = std::make_unique<LightingSystem>();

int main()
{
	GLFWwindow *window;

	// GLFW window
	if (!glfwInit())
		return -1;
	window = glfwCreateWindow(1280, 720, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// GLEW
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW failed to initialize" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	// GLFW callbacks
	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		inputSystem->OnKeyChanged(key, action);
	});
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        rendererSystem->SetRenderSize(width, height);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        inputSystem->OnMouseMoved(xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
       inputSystem->OnMouseButtonChanged(button, action);
    });

    // Debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *message,const void *userParam)
       {
            for (;length > 0; length--)
                std::cout << *(message++);

            std::cout << std::endl;
       }, nullptr);


    // Set up rendering
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    rendererSystem->SetRenderSize(width, height);

	// Camera
    Entity camera;
    camera
        .AddComponent<TransformComponent>()
        .AddComponent<CameraComponent>(ProjectionType::Perspective, 75.0f)
		.AddComponent<LightComponent>(LightComponent::LightType::Spot);
    rendererSystem->SetRenderCamera(camera.GetComponent<CameraComponent>());
	auto flashLight = camera.GetComponent<LightComponent>();
	flashLight->SetColor(glm::vec3(255, 243, 207) * 0.5f/100.0f);
	flashLight->SetDirection(glm::vec3(0, 0, -1));
	flashLight->SetSpotAngles(10, 90);

	// Materials
	auto shaderPbr = ShaderLoader::LoadShader("PBRShader.glsl");

	auto materialGround = Handle<Material>::Make(*shaderPbr);
	auto groundAlbedo = TextureLoader::LoadTexture("ForestGround/ForestGround_Albedo.png");
	auto groundDisplacement = TextureLoader::LoadTexture("ForestGround/ForestGround_Displacement.png");
	auto groundNormal = TextureLoader::LoadTexture("ForestGround/ForestGround_Normal.png");
	auto groundRoughness = TextureLoader::LoadTexture("ForestGround/ForestGround_Roughness.png");
	auto groundOcclusion = TextureLoader::LoadTexture("ForestGround/ForestGround_Occlusion.png");
	materialGround->Set("AlbedoTex", groundAlbedo.Access());
	materialGround->Set("NormalTex", groundNormal.Access());
	materialGround->Set("OcclusionTex", groundOcclusion.Access());
	materialGround->Set("UseNormalmap", true);
	materialGround->Set("HeightmapTex", groundDisplacement.Access());
	materialGround->Set("UseHeightmap", true);
	materialGround->Set("HeightmapDepth", 0.05f);
	materialGround->Set("TextureScale", 3.0f);
	materialGround->Set("NormalMapStrenght", 1.0f);

	auto materialStoneBricks = Handle<Material>::Make(*shaderPbr);
	auto stoneBricksAlbedo = TextureLoader::LoadTexture("StoneBricks/StoneBricks_Albedo.png");
	auto stoneBricksDisplacement = TextureLoader::LoadTexture("StoneBricks/StoneBricks_Displacement.png");
	auto stoneBricksNormal = TextureLoader::LoadTexture("StoneBricks/StoneBricks_Normal.png");
	auto stoneBricksRoughness = TextureLoader::LoadTexture("StoneBricks/StoneBricks_Roughness.png");
	auto stoneBricksOcclusion = TextureLoader::LoadTexture("StoneBricks/StoneBricks_Occlusion.png");
	materialStoneBricks->Set("AlbedoTex", stoneBricksAlbedo.Access());
	materialStoneBricks->Set("NormalTex", stoneBricksNormal.Access());
	materialStoneBricks->Set("OcclusionTex", stoneBricksOcclusion.Access());
	materialStoneBricks->Set("UseNormalmap", true);
	materialStoneBricks->Set("HeightmapTex", stoneBricksDisplacement.Access());
	materialStoneBricks->Set("UseHeightmap", true);
	materialStoneBricks->Set("HeightmapDepth", 0.1f);
	materialStoneBricks->Set("TextureScale", 4.0f);
	materialStoneBricks->Set("NormalMapStrenght", 2.0f);

	// Models
	ModelLoader::LoadModel("Plane.obj");
    Entity plane;
    plane
        .AddComponent<TransformComponent>()
        .AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Plane.obj"), *materialGround);
    plane.Transform()->Scale() = glm::vec3(15, 15, 15);
    plane.Transform()->Position() = glm::vec3(0, 0, 0);


    ModelLoader::LoadModel("Suzanne.obj");
    Entity suzanne;
    suzanne.AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Suzanne.obj"), *materialGround.Access());
    suzanne.Transform()->Position() = glm::vec3(1, 0.6, -1);
    suzanne.Transform()->Scale() = glm::vec3(0.4, 0.4, 0.4);

    ModelLoader::LoadModel("Cube.glb");
    Entity cube;
    cube.AddComponent<TransformComponent>()
		.AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Cube.glb"), *materialStoneBricks);
    cube.Transform()->Position() = glm::vec3(-0.8, 0.5, -1.3);


	// Lights
	auto emissionShader = ShaderLoader::LoadShader("EmissionShader.glsl");
	ModelLoader::LoadModel("Icosphere.obj");

	auto emissionMaterial0 = Handle<Material>::Make(*emissionShader);
	auto col0 = glm::vec3(214/255.0f, 96/255.0f, 151/255.0f) * 1.0f;
	Entity pointLight0;
	pointLight0
		.AddComponent<TransformComponent>()
		.AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Icosphere.obj"), *emissionMaterial0)
		.AddComponent<LightComponent>(LightComponent::LightType::Point);
	pointLight0.Transform()->Position() = glm::vec3(-1.5, 0.3, 0);
	pointLight0.Transform()->Scale() = glm::vec3(0.1);
	emissionMaterial0->Set("Color", glm::vec4(col0, 0));
	auto pl0 = pointLight0.GetComponent<LightComponent>();
	pl0->SetColor(col0);

	auto emissionMaterial1 = Handle<Material>::Make(*emissionShader);
	auto col1 = glm::vec3(96.0f/255.0f, 137.0f/255, 214.0f/255) * 1.0f;
	Entity pointLight1;
	pointLight1
		.AddComponent<TransformComponent>()
		.AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Icosphere.obj"), *emissionMaterial1)
		.AddComponent<LightComponent>(LightComponent::LightType::Point);
	pointLight1.Transform()->Scale() = glm::vec3(0.1);
	emissionMaterial1->Set("Color", glm::vec4(col1, 0));
	auto pl1 = pointLight1.GetComponent<LightComponent>();
	pl1->SetColor(col1);

	auto emissionMaterial2 = Handle<Material>::Make(*emissionShader);
	auto col2 = glm::vec3(0.8, 0.15, 0.2) * 1.0f;
	Entity spotLight0;
	spotLight0
		.AddComponent<TransformComponent>()
		.AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Icosphere.obj"), *emissionMaterial2)
		.AddComponent<LightComponent>(LightComponent::LightType::Spot);
	spotLight0.Transform()->Scale() = glm::vec3(0.1);
	spotLight0.Transform()->Position() = glm::vec3(0, 1.5, 0);
	emissionMaterial2->Set("Color", glm::vec4(col2, 0));
	auto pl2 = spotLight0.GetComponent<LightComponent>();
	pl2->SetColor(col2);
	pl2->SetSpotAngles(20, 80);
	pl2->SetDirection(glm::vec3(0.7, -0.3, 0));


	// Game loop
	bool mouse = true;
	while (!glfwWindowShouldClose(window))
	{
		// Update state
		timeKeeper->Update();

		if(inputSystem->IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mouse = false;
        }

		if(inputSystem->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			inputSystem->RestartRelativeMouse();
			mouse = true;
		}

		if(!mouse)
			inputSystem->RestartRelativeMouse();


        // Camera movement
        float rotationX, rotationY;
        inputSystem->GetRelativeMouse(rotationX, rotationY);
        inputSystem->RestartRelativeMouse();

        camera.Transform()->AngleAxis(-rotationX * timeKeeper->DeltaTime() * 3*1.77f, glm::vec3(0, 1, 0));
        camera.Transform()->AngleAxis(-rotationY * timeKeeper->DeltaTime() * 3, camera.Transform()->Right());

        float movementRight = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_D)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_A));
        float movementForward = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_W)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_S));
        float movementUp = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_SPACE)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_LEFT_SHIFT));

        glm::vec3 movementHorizontal = movementRight * camera.Transform()->Right() + movementForward * camera.Transform()->Forward();
        if(glm::length(movementHorizontal) > 0)
            movementHorizontal = glm::normalize(movementHorizontal);
        glm::vec3 movement = movementHorizontal + movementUp * camera.Transform()->Up();

        camera.Transform()->Position() += static_cast<float>(timeKeeper->DeltaTime() * 2) * movement;

		// Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pointLight0.Transform()->Position() = glm::vec3(sin(timeKeeper->TimeSinceStartup() * 1.5) * 2, 0.5, sin(timeKeeper->TimeSinceStartup() * 3) * 0.7);
		pointLight1.Transform()->Position() = glm::vec3(sin(timeKeeper->TimeSinceStartup() * 2), sin(timeKeeper->TimeSinceStartup() * 4) * 0.4 + 0.5, 0);
		spotLight0.Transform()->AngleAxis(timeKeeper->DeltaTime() * 80, glm::vec3(0, 1, 0));

		suzanne.Transform()->AngleAxis(30.0f * timeKeeper->DeltaTime(), glm::vec3(0, 1, 0));
		suzanne.Transform()->Scale() = glm::vec3(glm::sin(timeKeeper->TimeSinceStartup()), glm::sin(timeKeeper->TimeSinceStartup()), glm::sin(timeKeeper->TimeSinceStartup()));

		lightingSystem->UpdateLights();
        rendererSystem->Render();

		glfwSwapBuffers(window);

		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

