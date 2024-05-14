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
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Utils/CollisionTerrain.hpp"
#include "Utils/Spline.hpp"

std::unique_ptr<TimeKeeper> timeKeeper;
std::unique_ptr<InputSystem> inputSystem;
std::unique_ptr<RenderSystem> rendererSystem;
std::unique_ptr<LightingSystem> lightingSystem;

int main()
{
	// GLFW
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	GLFWwindow *mainWindow;
	mainWindow = glfwCreateWindow(1280, 720, "OGL_Engine", nullptr, nullptr);
	if (!mainWindow)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(mainWindow);

	// GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize" << std::endl;
		return -1;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Systems
	timeKeeper = std::make_unique<TimeKeeper>();
	inputSystem = std::make_unique<InputSystem>();
	rendererSystem = std::make_unique<RenderSystem>();
	lightingSystem = std::make_unique<LightingSystem>();

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
	ResourceDatabase::AddMesh(MeshFactory::CreateQuad());
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Plane.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Suzanne.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Cube.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Icosphere.obj"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Forest/Ground.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Forest/Ground_Collision.glb", true));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Forest/LakeWater.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Forest/Tree1.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Buoy.glb"));
	ResourceDatabase::AddMesh(ModelLoader::LoadModel("Campfire.glb"));

	// Shaders
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("Internal/EntityIdShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("Internal/EntityHighlightShader.glsl"));

	ResourceDatabase::AddShader(ShaderLoader::LoadShader("ShadowShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PointShadowShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PBRShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("EmissionShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("SkyboxShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("PostprocessShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("SimpleTransparentShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("WaterShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("FireShader.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("ParticleFlipbook.glsl"));
	ResourceDatabase::AddShader(ShaderLoader::LoadShader("VolumetricFogShader.glsl"));

	// Textures
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("white.png"));

	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Albedo.png"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Displacement.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Normal.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Roughness.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("ForestGround/ForestGround_Occlusion.png", {.sRGB = false}));

	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Bark01/Bark01_Albedo.png"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Bark01/Bark01_Displacement.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Bark01/Bark01_Normal.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Bark01/Bark01_Roughness.png", {.sRGB = false}));

	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Buoy/Buoy_Albedo.png"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Buoy/Buoy_Normal.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Buoy/Buoy_Roughness.png", {.sRGB = false}));

	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Campfire/Campfire_Albedo.png"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Campfire/Campfire_Normal.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Campfire/Campfire_Roughness.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Campfire/Campfire_Occlusion.png", {.sRGB = false}));

	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Albedo.png"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Displacement.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Normal.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Roughness.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("StoneBricks/StoneBricks_Occlusion.png", {.sRGB = false}));
	ResourceDatabase::AddTexture(TextureLoader::LoadCubemap("Skybox/Night.hdr", {false, false, Texture::Tiling::Extend}));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("FireNoiseSeamless.png"));
	ResourceDatabase::AddTexture(TextureLoader::LoadTexture2D("Flipbook/FireBall02_8x8.hdr"));

	// Materials
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("GroundMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("StoneBricksMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial1.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial2.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("EmissionMaterial3.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("FireMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("WaterMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("BarkMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("ParticleFlipbookMat.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("BuoyMaterial.mat"));
	ResourceDatabase::AddMaterial(MaterialLoader::LoadMaterial("CampfireMaterial.mat"));


	// Scene
	auto scene = Serializer::LoadScene("SpookyForest.scene");
	Entity& ground = *scene->GetEntity("Ground");
	Entity& emissiveSphere1 = *scene->GetEntity("EmissiveSphere1");
	Entity& emissiveSphere2 = *scene->GetEntity("EmissiveSphere2");

	Entity& buoy = *scene->GetEntity("Buoy");
	Entity& warningLight = *scene->GetEntity("WarningLight");
	warningLight.GetTransform()->SetParent(buoy.GetTransform());
	warningLight.GetTransform()->LocalPosition(glm::vec3(0, 3.75, 0));
	auto warningLightColor = warningLight.GetComponent<LightComponent>()->GetColor();

	Entity& fireball = *scene->GetEntity("Fireball");
	fireball.GetTransform()->Rotation(glm::quat(glm::vec3(0, -45, 0)));
	LightComponent& fireballLight = *scene->GetEntity("FireballLight")->GetComponent<LightComponent>();
	glm::vec3 fireballLightColor =fireballLight.GetColor();

	Entity& campfire = *scene->GetEntity("Campfire");
	Entity& fire = *scene->GetEntity("Fire");
	fire.GetTransform()->SetParent(campfire.GetTransform());
	fire.GetTransform()->LocalPosition(glm::vec3(0, 0.3, 0));

	Spline path1 ({
		glm::vec3(-46.763466, 1.604800, 38.400867),
		glm::vec3(-30.252951, 2.599903, 35.546146),
		glm::vec3(-16.164583, 2.506992, 35.887779),
		glm::vec3(-8.282252, 3.664126, 45.020203),
		glm::vec3(-2.339834, 3.370311, 35.728607),
		glm::vec3(4.756345, 3.972067, 33.395844),
		glm::vec3(8.889107, 3.847589, 25.879944),
		glm::vec3(12.823547, 3.133041, 11.608407),
		glm::vec3(22.913528, 1.412410, -11.244169),
		glm::vec3(24.035505, 4.074424, -42.231049),
		glm::vec3(2.842738, 3.862304, -49.383217),
		glm::vec3(-14.745708, 3.735934, -50.389236),
		glm::vec3(-31.436272, 6.579220, -49.870770),
		glm::vec3(-59.219330, 22.259750, -53.168964),
		glm::vec3(-67.896065, 14.807197, -34.900658),
		glm::vec3(-68.334106, 9.431996, -19.093090),
		glm::vec3(-58.151402, 3.607426, -7.889922),
		glm::vec3(-57.247894, 0.977964, 1.966164),
		glm::vec3(-58.355469, 0.306650, 15.580692),
		glm::vec3(-57.175400, 10.172410, 22.823545),
		glm::vec3(-56.632530, 8.739169, 36.918461)
	}, true);

	Spline path2 ({
		glm::vec3(-20.324287, 4.320406, 47.003925),
		glm::vec3(-39.547157, 1.574403, 39.784435),
		glm::vec3(-53.356712, -0.631056, 30.385187),
		glm::vec3(-63.848427, 0.148807, 15.595203),
		glm::vec3(-60.036579, 2.561289, -9.130140),
		glm::vec3(-41.664307, 5.773416, -23.148098),
		glm::vec3(-37.194904, 4.922056, -14.466686),
		glm::vec3(-15.324898, 2.610783, 10.671150),
		glm::vec3(-7.846649, 1.523810, 24.275705),
		glm::vec3(4.490672, 3.153982, 24.140432),
		glm::vec3(16.087767, 3.470337, 35.054230),
		glm::vec3(-0.304375, 2.642592, 47.540894)
	}, true);

	Spline path3({
			glm::vec3(11.735162, 3.099527, 4.173303),
			glm::vec3(15.394011, 1.708269, -3.791292),
			glm::vec3(8.157375, 1.479180, -7.879596),
			glm::vec3(1.626803, 1.796493, -6.859668),
			glm::vec3(0.137587, 2.063640, -11.803920),
			glm::vec3(5.999215, 1.707160, -15.201338),
			glm::vec3(4.225876, 6.538656, -9.954954),
			glm::vec3(-21.940914, 6.588815, -16.913481),
			glm::vec3(-16.417822, 3.692607, -3.661718),
			glm::vec3(-1.126975, 2.971200, 1.317145)

		}, true);

	CollisionTerrain terrain(150.0f, ResourceDatabase::GetMesh("Forest/Ground_Collision.glb")->GetVertices(), ground.GetTransform()->WorldToModel());

	// Other
	Entity camera("Camera");
	camera
		.AddComponent<TransformComponent>()
		.AddComponent<CameraComponent>(CameraComponent::ProjectionType::Perspective, 65);
	camera.GetTransform()->Position(glm::vec3(-48.0f, -1.0f, 23.0f));
	camera.GetTransform()->AngleAxis(-90, glm::vec3(0, 1, 0));
	rendererSystem->SetRenderCamera(camera.GetComponent<CameraComponent>());

	Entity flashlight;
	flashlight
	    .AddComponent<TransformComponent>()
	    .AddComponent<LightComponent>(LightComponent::Type::Spot);
	flashlight.GetTransform()->SetParent(camera.GetTransform());
	flashlight.GetTransform()->LocalPosition(glm::vec3(0.2f, -0.5f, 0.1f));
	flashlight.GetTransform()->LocalRotation(glm::vec3(0));
	flashlight.GetComponent<LightComponent>()->SetSpotAngles(15, 120);
	flashlight.GetComponent<LightComponent>()->SetShadowCasting(true);
	flashlight.GetComponent<LightComponent>()->SetColor(glm::vec3(0.5, 0.6, 1) * 50.0f);
	auto flashlightCol = flashlight.GetComponent<LightComponent>()->GetColor();

	Entity skybox;
	Material skyboxMat(*ResourceDatabase::GetShader("SkyboxShader.glsl"), "SkyboxMat");
	skybox.AddComponent<TransformComponent>().AddComponent<RendererComponent>(ResourceDatabase::GetMesh("Cube.glb"), &skyboxMat);
	skyboxMat.Set("SkyboxTex", ResourceDatabase::GetTexture("Skybox/Night"));


	// Game loop
	bool editMode = false;
	bool isPlayerCam = true;
	bool flashlightEnabled = true;
	bool gravityEnabled = true;
	float fireballTravel = 1.0f;
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	inputSystem->RestartRelativeMouse();
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Update state
		timeKeeper->Update();
		glfwPollEvents();

		// Edit mode
		if(InputSystem::GetKeyPress(GLFW_KEY_ESCAPE))
		{
			editMode = !editMode;
			rendererSystem->EnableEntityIds(editMode);
			glfwSetInputMode(mainWindow, GLFW_CURSOR, editMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
		if (editMode)
		{
			inputSystem->RestartRelativeMouse();

			auto [x, y] = InputSystem::GetMousePosition();
			auto id = RenderSystem::GetEntityId(x, y);
			Entity* ent = Entity::GetById(id);
			rendererSystem->SetHighlightEntity(ent);

			if (InputSystem::GetMouseButtonPress(GLFW_MOUSE_BUTTON_1))
			{
				if (ent)
				{
					if (ent->GetName() == "Buoy")
					{
						auto entLight = warningLight.GetComponent<LightComponent>();
						if (entLight->GetColor() == glm::vec3(0))
							entLight->SetColor(warningLightColor);
						else
							entLight->SetColor(glm::vec3(0));
					}
					else if (ent->GetName() == "Fireball")
					{
						fireballTravel = 0.0f;
					}
					else if (ent->GetName() == "Campfire")
					{
						auto localPos = fire.GetTransform()->LocalPosition();
						fire.GetTransform()->LocalPosition(glm::vec3(localPos.x, localPos.y + (localPos.y < 10.0f ? 100.0f : -100.0f), localPos.z));
					}
				}
			}
			else if (InputSystem::GetKeyPress(GLFW_KEY_M))
			{
				if (ent->GetTransform()->GetParent())
					ent->GetTransform()->SetParent(nullptr);
				else
					ent->GetTransform()->SetParent(camera.GetTransform());
			}
		}

		// Camera positions
		int selectedCamera = 0;
		if (InputSystem::GetKeyPress(GLFW_KEY_F1))
			selectedCamera = 1;
		if (InputSystem::GetKeyPress(GLFW_KEY_F2))
			selectedCamera = 2;
		if (InputSystem::GetKeyPress(GLFW_KEY_F3))
			selectedCamera = 3;
		if (InputSystem::GetKeyPress(GLFW_KEY_F4))
			selectedCamera = 4;
		if (InputSystem::GetKeyPress(GLFW_KEY_F5))
			selectedCamera = 5;
		if (InputSystem::GetKeyPress(GLFW_KEY_F6))
			selectedCamera = 6;
		if (selectedCamera != 0)
		{
			// Free or set
			if (selectedCamera < 5)
			{
				flashlight.GetComponent<LightComponent>()->SetColor(glm::vec3(0));
				isPlayerCam = false;

				if (selectedCamera < 4)
				{
					camera.GetTransform()->SetParent(nullptr);
					auto camPos = scene->GetEntity(std::format("CameraPos{}", selectedCamera))->GetTransform();
					camera.GetTransform()->AlignWith(*camPos);
				}
				else
				{
					if (camera.GetTransform()->GetParent() == emissiveSphere1.GetTransform())
						camera.GetTransform()->SetParent(emissiveSphere2.GetTransform());
					else
						camera.GetTransform()->SetParent(emissiveSphere1.GetTransform());
					camera.GetTransform()->LocalPosition(glm::vec3(0, 0, 0));
					camera.GetTransform()->LocalRotation(glm::quat(glm::vec3(0)));
				}
			}
			else
			{
				camera.GetTransform()->SetParent(nullptr);
				flashlight.GetComponent<LightComponent>()->SetColor(flashlightEnabled ? flashlightCol : glm::vec3(0));
				isPlayerCam = true;
				gravityEnabled = selectedCamera == 6;
				inputSystem->RestartRelativeMouse();
			}
		}

		// Flashlight
		if (InputSystem::GetKeyPress(GLFW_KEY_F))
		{
			flashlightEnabled = !flashlightEnabled;
			flashlight.GetComponent<LightComponent>()->SetColor(flashlightEnabled ? flashlightCol : glm::vec3(0));
		}

		if (InputSystem::GetKeyPress(GLFW_KEY_P))
		{
			std::cout << to_string(camera.GetTransform()->Position()) << std::endl;
		}


        // Camera movement
		if (isPlayerCam)
		{
			float rotationX, rotationY;
			inputSystem->GetRelativeMouse(rotationX, rotationY);
			inputSystem->RestartRelativeMouse();
			camera.GetTransform()->AngleAxis(-rotationX * TimeKeeper::DeltaTime() * 4*1.77f, glm::vec3(0, 1, 0));
			camera.GetTransform()->AngleAxis(-rotationY * TimeKeeper::DeltaTime() * 4, camera.GetTransform()->Right());

			const auto boundsMin = glm::vec3(-75, -5, -75);
			const auto boundsMax = glm::vec3(75, 20, 75);
			float movementRight = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_D)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_A));
			float movementForward = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_W)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_S));
			float movementUp = static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_SPACE)) - static_cast<float>(inputSystem->IsKeyPressed(GLFW_KEY_LEFT_SHIFT));
			glm::vec3 movementHorizontal = movementRight * camera.GetTransform()->Right() + movementForward * camera.GetTransform()->Forward();
			if(glm::length(movementHorizontal) > 0)
				movementHorizontal = glm::normalize(movementHorizontal);
			glm::vec3 movement = movementHorizontal + movementUp * camera.GetTransform()->Up();
			auto newPos = camera.GetTransform()->Position() + static_cast<float>(TimeKeeper::DeltaTime() * 8) * movement;
			newPos = clamp(newPos, boundsMin, boundsMax);
			if (gravityEnabled)
				newPos = glm::vec3(newPos.x, terrain.GroundAt(newPos).y + 1.75f, newPos.z);
			camera.GetTransform()->Position(newPos);
		}

		// Objects
		auto es1p = path1.Evaluate(TimeKeeper::TimeSinceStartup() / 70);
		auto es1d = normalize(path1.Evaluate(TimeKeeper::TimeSinceStartup() / 70 + 0.01f) - es1p);
		emissiveSphere1.GetTransform()->Position(es1p);
		emissiveSphere1.GetTransform()->LookAt(es1d, glm::vec3(0, 1, 0));

		auto es2p = path2.Evaluate(TimeKeeper::TimeSinceStartup() / 70 + 0.6f);
		auto es2d = normalize(path2.Evaluate(TimeKeeper::TimeSinceStartup() / 70 + 0.6f + 0.01f) - es2p);
		emissiveSphere2.GetTransform()->Position(es2p);
		emissiveSphere2.GetTransform()->LookAt(es2d, glm::vec3(0, 1, 0));

		// Buoy
		float buoyRotX = sin(TimeKeeper::TimeSinceStartup()* 1.5) * 0.12f;
		float buoyRotZ = sin(TimeKeeper::TimeSinceStartup()* 1.7f) * 0.09f;
		buoy.GetTransform()->Rotation(glm::quat(glm::vec3(buoyRotX, 0, buoyRotZ)));
		warningLight.GetTransform()->LocalRotation(glm::angleAxis(static_cast<float>(TimeKeeper::TimeSinceStartup()) * 1.5f, glm::vec3(0, 1, 0)));

		// Fireball
		if (fireballTravel < 1.0f)
		{
			fireballTravel += TimeKeeper::DeltaTime() * 0.08f;
			fireball.GetTransform()->Position(path3.Evaluate(fireballTravel));
		}
		float shakeSpeed = 3.5f;
		float shakeStrength = 0.01f;
		fireballLight.GetTransform()->Position(fireball.GetTransform()->Position() + glm::vec3(sin(TimeKeeper::TimeSinceStartup() * shakeSpeed * 4.5) * shakeStrength, sin(TimeKeeper::TimeSinceStartup() * shakeStrength * 3.75f + 5.0f) * shakeStrength, sin(TimeKeeper::TimeSinceStartup() * shakeSpeed * 3.0f + 14.0f) * shakeStrength));
		float lightMult = (sin(TimeKeeper::TimeSinceStartup() * 12) + 1) / 2;
		lightMult *= (sin(TimeKeeper::TimeSinceStartup() * 21.5f) + 1) / 4 + 0.5;
		fireballLight.SetColor(fireballLightColor * (2.0f + lightMult * 0.8f - 0.6f));

		// Render
		lightingSystem->UpdateLights();
        rendererSystem->Render();
		inputSystem->ClearPresses();
		glfwSwapBuffers(mainWindow);
		glFinish();
	}


	glfwTerminate();
	return 0;
}

