cmake_minimum_required(VERSION 3.25)
project(OGL_Engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        src/Application.cpp

        # Core
        src/Core/Handle.hpp
        src/Core/HybridSingleton.hpp
        src/Core/Singleton.hpp

        # IO
        src/IO/YAMLExtensions.hpp src/IO/YAMLExtensions.cpp
        src/IO/MaterialLoader.hpp src/IO/MaterialLoader.cpp
        src/IO/Serializer.hpp src/IO/Serializer.cpp
        src/IO/TextureLoader.hpp src/IO/TextureLoader.cpp
        src/IO/ShaderLoader.hpp src/IO/ShaderLoader.cpp
        src/IO/ModelLoader.hpp src/IO/ModelLoader.cpp
        src/IO/MeshFactory.hpp src/IO/MeshFactory.cpp

        # Components
        src/Components/CameraComponent.hpp src/Components/CameraComponent.cpp
        src/Components/Component.hpp src/Components/Component.cpp
        src/Components/RendererComponent.hpp src/Components/RendererComponent.cpp
        src/Components/TransformComponent.hpp src/Components/TransformComponent.cpp
        src/Components/LightComponent.hpp src/Components/LightComponent.cpp

        # Scene
        src/Scene/Entity.hpp src/Scene/Entity.cpp
        src/Scene/Scene.hpp src/Scene/Scene.cpp

        # Systems
        src/Systems/InputSystem.hpp src/Systems/InputSystem.cpp
        src/Systems/RenderSystem.hpp src/Systems/RenderSystem.cpp
        src/Systems/ResourceDatabase.hpp src/Systems/ResourceDatabase.cpp
        src/Systems/TimeKeeper.hpp src/Systems/TimeKeeper.cpp
        src/Systems/LightingSystem.hpp src/Systems/LightingSystem.cpp

        # OGL
        src/OGL/Graphics.hpp src/OGL/Graphics.cpp
        src/OGL/IBindable.hpp
        src/OGL/IndexBuffer.hpp src/OGL/IndexBuffer.cpp
        src/OGL/VertexArray.hpp src/OGL/VertexArray.cpp
        src/OGL/VertexAttributes.hpp src/OGL/VertexAttributes.cpp
        src/OGL/VertexBuffer.hpp src/OGL/VertexBuffer.cpp
        src/OGL/UniformBuffer.hpp src/OGL/UniformBuffer.cpp
        src/OGL/ShaderStructs.hpp src/OGL/ShaderStructs.cpp
        src/OGL/FrameBuffer.hpp src/OGL/FrameBuffer.cpp

        # Resources
        src/Resources/Resource.hpp src/Resources/Resource.cpp
        src/Resources/Mesh.hpp src/Resources/Mesh.cpp
        src/Resources/Shader.hpp src/Resources/Shader.cpp
        src/Resources/Material.hpp src/Resources/Material.cpp
        src/Resources/Texture.hpp src/Resources/Texture.cpp

        # Utils
        src/Utils/Spline.hpp src/Utils/Spline.cpp
)

# Libraries
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)