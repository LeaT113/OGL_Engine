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

    const float positions[] = {
            -0.5, 0, 0.5,
            0.5, 0, 0.5,
            0.5, 0, -0.5,
            -0.5, 0, -0.5
    };
    const float normals[] = {
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0
    };
    const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
    };
    ResourceDatabase::AddMesh("Ground", 4, positions, normals, 2, indices);
    Entity ground;
    ground
        .AddComponent<TransformComponent>()
        .AddComponent<RendererComponent>(*ResourceDatabase::GetMesh("Ground"), material);
    ground.Transform()->Scale() = glm::vec3(5, 5, 5);
    ground.Transform()->Position() = glm::vec3(0, 0, 0);



    const float cubePositions[] = {
            0.500000, 0.500000, -0.500000,
            0.500000, -0.500000, -0.500000,
            0.500000, 0.500000, 0.500000,
            0.500000, -0.500000, 0.500000,
            -0.500000, 0.500000, -0.500000,
            -0.500000, -0.500000, -0.500000,
            -0.500000, 0.500000, 0.500000,
            -0.500000, -0.500000, 0.500000,
    };
    const unsigned int cubeIndices[] = {
            4, 2, 0,
            2, 7, 3,
            6, 5, 7,
            1, 7, 5,
            0, 3, 1,
            4, 1, 5,
            4, 6, 2,
            2, 6, 7,
            6, 4, 5,
            1, 3, 7,
            0, 2, 3,
            4, 0, 1,
    };

    ResourceDatabase::LoadMesh("Suzanne.obj");
    Entity suzanne;
    suzanne
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(*ResourceDatabase::GetMesh("Suzanne.obj"), material);
    suzanne.Transform()->Position() = glm::vec3(0, 0.8, 0);
    suzanne.Transform()->Scale() = glm::vec3(0.4, 0.4, 0.4);

    camera.Transform()->Position() = glm::vec3(0, 0.8, 1);
    //camera.Transform()->AngleAxis(-20, camera.Transform()->Right());

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Update state
		timeKeeper->Update();

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

        //camera.Transform()->Position() = glm::vec3(sin(timeKeeper->TimeSinceStartup()*0.5f) * 0.2, 0, 0);

        suzanne.Transform()->AngleAxis(timeKeeper->DeltaTime() * 50, glm::vec3(0, 1, 0));

        rendererSystem->Render();

		glfwSwapBuffers(window);

		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

