#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Systems/InputSystem.hpp"
#include "Systems/TimeKeeper.hpp"
#include "Entity/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "Systems/ResourceDatabase.hpp"
#include "Systems/RenderSystem.hpp"


std::unique_ptr<TimeKeeper> timeKeeper = std::make_unique<TimeKeeper>();
std::unique_ptr<InputSystem> inputSystem = std::make_unique<InputSystem>();
std::unique_ptr<RenderSystem> rendererSystem = std::make_unique<RenderSystem>();

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


	// Geometry
    ResourceDatabase::LoadShader("PhongShader.glsl");
    auto shader = ResourceDatabase::GetShader("PhongShader.glsl");
    MaterialData material(*shader);


    ResourceDatabase::LoadMesh("Plane.obj");
    Entity plane;
    plane
        .AddComponent<TransformComponent>()
        .AddComponent<RendererComponent>(*ResourceDatabase::GetMesh("Plane.obj"), material);
    plane.Transform()->Scale() = glm::vec3(15, 15, 15);
    plane.Transform()->Position() = glm::vec3(0, 0, 0);

    ResourceDatabase::LoadMesh("Suzanne.obj");
    Entity suzanne;
    suzanne
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(*ResourceDatabase::GetMesh("Suzanne.obj"), material);
    suzanne.Transform()->Position() = glm::vec3(1, 0.6, -1);
    suzanne.Transform()->Scale() = glm::vec3(0.4, 0.4, 0.4);

    ResourceDatabase::LoadMesh("Cube.obj");
    Entity cube;
    cube
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(*ResourceDatabase::GetMesh("Cube.obj"), material);
    cube.Transform()->Position() = glm::vec3(-0.8, 0.5, -1.3);


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

        suzanne.Transform()->AngleAxis(timeKeeper->DeltaTime() * 50, glm::vec3(0, 1, 0));

        rendererSystem->Render();

		glfwSwapBuffers(window);

		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

