#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Systems/InputSystem.hpp"
#include "Systems/TimeKeeper.hpp"
#include "Scene/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "IO/MaterialLoader.hpp"
#include "IO/ModelLoader.hpp"
#include "IO/Serializer.hpp"
#include "IO/ShaderLoader.hpp"
#include "IO/TextureLoader.hpp"
#include "OGL/Graphics.hpp"
#include "Systems/ResourceDatabase.hpp"
#include "Systems/RenderSystem.hpp"
#include "Scene/Scene.hpp"
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
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    rendererSystem->SetRenderSize(width, height);


		// Resources
	// Meshes
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Plane.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Suzanne.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Cube.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Icosphere.obj"));

	// Shaders
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PBRShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("EmissionShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("SkyboxShader.glsl"));

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
	ResourceDatabase::AddTexture(TextureLoader::LoadCubemap("Skybox/Night", {false, true, Texture::Repeat::Extend}));

	// Materials
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("GroundMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("StoneBricksMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial1.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial2.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial3.mat"));


	// Scene
	auto scene = Serializer::LoadScene("DemoScene.scene");
	Entity& camera = *scene->GetEntity("Camera");
	rendererSystem->SetRenderCamera(camera.GetComponent<CameraComponent>());
	Entity& emissiveSphere1 = *scene->GetEntity("EmissiveSphere1");
	Entity& emissiveSphere2 = *scene->GetEntity("EmissiveSphere2");
	Entity& warningLight = *scene->GetEntity("WarningLight");

	Entity skybox;
	Material skyboxMat(*ResourceDatabase::GetShader("SkyboxShader.glsl"), "SkyboxMax");
	skybox.AddComponent<TransformComponent>().AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Cube.glb"), &skyboxMat);
	skyboxMat.Set("SkyboxTex", ResourceDatabase::GetTexture("Skybox/Night"));

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

		emissiveSphere1.Transform()->Position() = glm::vec3(sin(timeKeeper->TimeSinceStartup() * 1.5) * 2, 0.5, sin(timeKeeper->TimeSinceStartup() * 3) * 0.7);
		emissiveSphere2.Transform()->Position() = glm::vec3(sin(timeKeeper->TimeSinceStartup() * 2), sin(timeKeeper->TimeSinceStartup() * 4) * 0.4 + 0.5, 0);
		warningLight.Transform()->AngleAxis(timeKeeper->DeltaTime() * 80, glm::vec3(0, 1, 0));

		//suzanne.Transform()->AngleAxis(30.0f * timeKeeper->DeltaTime(), glm::vec3(0, 1, 0));
		//suzanne.Transform()->Scale() = glm::vec3(glm::sin(timeKeeper->TimeSinceStartup()), glm::sin(timeKeeper->TimeSinceStartup()), glm::sin(timeKeeper->TimeSinceStartup()));

		lightingSystem->UpdateLights();
        rendererSystem->Render();

		glfwSwapBuffers(window);

		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

