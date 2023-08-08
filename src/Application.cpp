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


	// GLEW
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW failed to initialize" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	// Window callbacks
	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		inputSystem->OnKeyChanged(key, action);
	});
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        rendererSystem->SetRenderSize(width, height);
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
        .AddComponent<CameraComponent>(ProjectionType::Perspective, 90.0f);
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
    /*
    ResourceDatabase::LoadMesh("Suzanne.obj");
    Entity suzanne;
    suzanne
            .AddComponent<TransformComponent>()
            .AddComponent<RendererComponent>(*ResourceDatabase::GetMesh("Suzanne.obj"), material);
    suzanne.Transform()->Position() = glm::vec3(0, 0.5, 0);
    suzanne.Transform()->Scale() = glm::vec3(0.4, 0.4, 0.4);*/

    camera.Transform()->Position() = glm::vec3(0, 2, 0);
    camera.Transform()->AngleAxis(-90, camera.Transform()->Right());

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Update state
		timeKeeper->Update();

		// Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //camera.Transform()->Position() = glm::vec3(sin(timeKeeper->TimeSinceStartup()*0.5f) * 0.2, 0, 0);

        //suzanne.Transform()->AngleAxis(timeKeeper->DeltaTime() * 50, glm::vec3(0, 1, 0));

        rendererSystem->Render();

		glfwSwapBuffers(window);

		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

