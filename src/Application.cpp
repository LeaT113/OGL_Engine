#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <iomanip>
#include "Systems/InputSystem.hpp"
#include "Systems/TimeKeeper.hpp"
#include "Entity/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "Systems/MeshDatabase.hpp"


std::unique_ptr<InputSystem> inputSystem;
std::unique_ptr<TimeKeeper> timeKeeper;


int main()
{
	Entity camera;
	camera.AddComponent<TransformComponent>()
	        .AddComponent<CameraComponent>(ProjectionType::Perspective, 90.0f);

	GLFWwindow *window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW failed to initialize" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	// Initialize systems
	timeKeeper = std::make_unique<TimeKeeper>();

	inputSystem = std::make_unique<InputSystem>();
	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		inputSystem->OnKeyChanged(key, action);
	});

	// Test model loading
	MeshDatabase::LoadMesh("Suzzane.obj");
	std::cout << MeshDatabase::GetMesh("Suzzane.obj") << std::endl;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Update state
		timeKeeper->Update();
		//std::cout << std::setprecision(1) << std::fixed << timeKeeper->TimeSinceStartup() << "s | " << timeKeeper->DeltaTime() * 1000 << "ms | " << 1 / timeKeeper->DeltaTime() << "FPS" << std::endl;


		// Render
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		glfwSwapBuffers(window);


		// Get input
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}
