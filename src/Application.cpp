#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Systems/InputSystem.hpp"
#include "Systems/TimeKeeper.hpp"
#include "Scene/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "IO/MaterialLoader.hpp"
#include "IO/MeshFactory.hpp"
#include "IO/ModelLoader.hpp"
#include "IO/Serializer.hpp"
#include "IO/ShaderLoader.hpp"
#include "IO/TextureLoader.hpp"
#include "OGL/Graphics.hpp"
#include "Systems/ResourceDatabase.hpp"
#include "Systems/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Systems/LightingSystem.hpp"

std::unique_ptr<TimeKeeper> timeKeeper;
std::unique_ptr<InputSystem> inputSystem;
std::unique_ptr<RenderSystem> rendererSystem;
std::unique_ptr<LightingSystem> lightingSystem;

int main()
{
	// Systems
	timeKeeper = std::make_unique<TimeKeeper>();
	inputSystem = std::make_unique<InputSystem>();
	rendererSystem = std::make_unique<RenderSystem>();
	lightingSystem = std::make_unique<LightingSystem>();

	// GLFW window
	if (!glfwInit())
		return -1;
	GLFWwindow *mainWindow;
	mainWindow = glfwCreateWindow(1280, 720, "OGL_Engine", nullptr, nullptr);
	if (!mainWindow)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(mainWindow);
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize" << std::endl;
		return -1;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	// GLFW callbacks
	glfwSetFramebufferSizeCallback(mainWindow, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		rendererSystem->SetRenderSize(width, height);
	});
	glfwSetKeyCallback(mainWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		inputSystem->OnKeyChanged(key, action);
	});
    glfwSetCursorPosCallback(mainWindow, [](GLFWwindow* window, double xpos, double ypos) {
        inputSystem->OnMouseMoved(xpos, ypos);
    });
    glfwSetMouseButtonCallback(mainWindow, [](GLFWwindow* window, int button, int action, int mods) {
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
    int width, height;
    glfwGetFramebufferSize(mainWindow, &width, &height);
    rendererSystem->SetRenderSize(width, height);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// Resources
	// Meshes
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Plane.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Suzanne.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Cube.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Icosphere.obj"));
	ResourceDatabase::AddMesh(MeshFactory::CreateQuad());

	// Shaders
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("ShadowShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PointShadowShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PBRShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("EmissionShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("SkyboxShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PostprocessShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("SimpleTransparentShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("WaterShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("FireShader.glsl"));

	// Textures
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Albedo"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Displacement", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Normal", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Roughness", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Occlusion", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Albedo"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Displacement", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Normal", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Roughness", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Occlusion", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadCubemap("Skybox/Night", {false, true, Texture::Tiling::Extend}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("FireNoiseSeamless"));

	// Materials
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("GroundMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("StoneBricksMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial1.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial2.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial3.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("FireMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("WaterMaterial.mat"));


	// Scene
	auto scene = Serializer::LoadScene("DemoScene.scene");
	Entity& emissiveSphere1 = *scene->GetEntity("EmissiveSphere1");
	Entity& emissiveSphere2 = *scene->GetEntity("EmissiveSphere2");
	Entity& warningLight = *scene->GetEntity("WarningLight");

	Entity camera("Camera");
	camera
		.AddComponent<TransformComponent>()
		.AddComponent<CameraComponent>(CameraComponent::ProjectionType::Perspective, 75);
	camera.GetTransform()->Position(glm::vec3(-2, 1.6, -0.5));
	camera.GetTransform()->AngleAxis(45, glm::vec3(0, 1, 0));
	rendererSystem->SetRenderCamera(camera.GetComponent<CameraComponent>());

	Entity flashlight;
	flashlight
	    .AddComponent<TransformComponent>()
	    .AddComponent<LightComponent>(LightComponent::Type::Spot);
	flashlight.GetTransform()->SetParent(camera.GetTransform());
	flashlight.GetTransform()->LocalPosition(glm::vec3(0.2f, -0.5f, 0.1f));
	flashlight.GetTransform()->LocalRotation(glm::vec3(0));
	flashlight.GetComponent<LightComponent>()->SetSpotAngles(10, 90);
	flashlight.GetComponent<LightComponent>()->SetShadowCasting(true);
	flashlight.GetComponent<LightComponent>()->SetColor(glm::vec3(2));

	Entity skybox;
	Material skyboxMat(*ResourceDatabase::GetShader("SkyboxShader.glsl"), "SkyboxMat");
	skybox.AddComponent<TransformComponent>().AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Cube.glb"), &skyboxMat);
	skyboxMat.Set("SkyboxTex", ResourceDatabase::GetTexture("Skybox/Night"));


	// Game loop
	bool mouse = true;
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Update state
		timeKeeper->Update();

		if(inputSystem->IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mouse = false;
        }

		if(inputSystem->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
		{
			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			inputSystem->RestartRelativeMouse();
			mouse = true;
		}

		if(!mouse)
			inputSystem->RestartRelativeMouse();


        // Camera movement
        float rotationX, rotationY;
        inputSystem->GetRelativeMouse(rotationX, rotationY);
        inputSystem->RestartRelativeMouse();

        camera.GetTransform()->AngleAxis(-rotationX * TimeKeeper::DeltaTime() * 3*1.77f, glm::vec3(0, 1, 0));
        camera.GetTransform()->AngleAxis(-rotationY * TimeKeeper::DeltaTime() * 3, camera.GetTransform()->Right());

        float movementRight = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_D)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_A));
        float movementForward = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_W)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_S));
        float movementUp = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_SPACE)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_LEFT_SHIFT));

        glm::vec3 movementHorizontal = movementRight * camera.GetTransform()->Right() + movementForward * camera.GetTransform()->Forward();
        if(glm::length(movementHorizontal) > 0)
            movementHorizontal = glm::normalize(movementHorizontal);
        glm::vec3 movement = movementHorizontal + movementUp * camera.GetTransform()->Up();

        camera.GetTransform()->Position(camera.GetTransform()->Position() + static_cast<float>(TimeKeeper::DeltaTime() * 2) * movement);

		// Objects
		emissiveSphere1.GetTransform()->Position(glm::vec3(sin(timeKeeper->TimeSinceStartup() * 1.5) * 2, 0.5, sin(timeKeeper->TimeSinceStartup() * 3) * 0.7));
		emissiveSphere2.GetTransform()->Position(glm::vec3(sin(timeKeeper->TimeSinceStartup() * 2), sin(timeKeeper->TimeSinceStartup() * 4) * 0.4 + 0.5, 0));
		warningLight.GetTransform()->AngleAxis(TimeKeeper::DeltaTime() * 80, glm::vec3(0, 1, 0));

		// Render
		lightingSystem->UpdateLights();
        rendererSystem->Render();
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

