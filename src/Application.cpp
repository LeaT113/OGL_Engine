#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Systems/InputSystem.hpp"
#include "Systems/TimeKeeper.hpp"
#include "Entity/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Systems/ResourceDatabase.hpp"
#include "Systems/RenderSystem.hpp"
#include "Resources/ModelLoader.hpp"
#include "Resources/ShaderLoader.hpp"
#include "Resources/Material.hpp"
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

    Entity camera;
    camera
        .AddComponent<TransformComponent>()
        .AddComponent<CameraComponent>(ProjectionType::Perspective, 75.0f);
    rendererSystem->SetRenderCamera(camera.GetComponent<CameraComponent>());

    auto s = ShaderLoader::LoadShader("PhongShader.glsl");
    auto material = Handle<Material>::Make(*s.Access());
    ModelLoader ml;

    ml.LoadModel("Plane.obj");
    Entity plane;
    plane
        .AddComponent<TransformComponent>()
        .AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Plane.obj"), *material.Access());
    plane.Transform()->Scale() = glm::vec3(15, 15, 15);
    plane.Transform()->Position() = glm::vec3(0, 0, 0);

    ml.LoadModel("Suzanne.obj");
    Entity suzanne;
    suzanne
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Suzanne.obj"), *material.Access());
    suzanne.Transform()->Position() = glm::vec3(1, 0.6, -1);
    suzanne.Transform()->Scale() = glm::vec3(0.4, 0.4, 0.4);

    ml.LoadModel("Cube.obj");
    Entity cube;
    cube
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Cube.obj"), *material.Access());
    cube.Transform()->Position() = glm::vec3(-0.8, 0.5, -1.3);

    ml.LoadModel("MaterialTest.obj");
    Entity materialTest;
    materialTest
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(ResourceDatabase::GetMesh("MaterialTest.obj"), *material.Access());
    materialTest.Transform()->Position() = glm::vec3(-1, 0.4, 1.3);
    materialTest.Transform()->Scale() = glm::vec3(0.4);

	auto emissionShader = ShaderLoader::LoadShader("EmissionShader.glsl");
	auto emissionMaterial0 = Handle<Material>::Make(*emissionShader);
	auto col0 = glm::vec3(214/255.0f, 96/255.0f, 151/255.0f);
	ml.LoadModel("Icosphere.obj");
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
	auto col1 = glm::vec3(96.0f/255.0f, 137.0f/255, 214.0f/255);
	Entity pointLight1;
	pointLight1
		.AddComponent<TransformComponent>()
		.AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Icosphere.obj"), *emissionMaterial1)
		.AddComponent<LightComponent>(LightComponent::LightType::Point);
	pointLight1.Transform()->Scale() = glm::vec3(0.1);
	emissionMaterial1->Set("Color", glm::vec4(col1, 0));
	auto pl1 = pointLight1.GetComponent<LightComponent>();
	pl1->SetColor(col1);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Update state
		timeKeeper->Update();

        if(inputSystem->IsKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if(inputSystem->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

		lightingSystem->UpdateLights();
        rendererSystem->Render();

		glfwSwapBuffers(window);

		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

