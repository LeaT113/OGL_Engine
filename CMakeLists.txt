cmake_minimum_required(VERSION 3.25)
project(OGL_Engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        src/Application.cpp

        # Core
        src/Core/Handle.hpp
        src/Core/HybridSingleton.hpp
        src/Core/Singleton.hpp

        # Components
        src/Components/CameraComponent.hpp src/Components/CameraComponent.cpp
        src/Components/Component.hpp src/Components/Component.cpp
        src/Components/RendererComponent.hpp src/Components/RendererComponent.cpp
        src/Components/TransformComponent.hpp src/Components/TransformComponent.cpp

        # Data
        src/Data/MaterialData.hpp src/Data/MaterialData.cpp
        src/Data/ShaderData.hpp src/Data/ShaderData.cpp

        # Entity
        src/Entity/Entity.hpp src/Entity/Entity.cpp

        # Systems
        src/Systems/InputSystem.hpp src/Systems/InputSystem.cpp
        src/Systems/RenderSystem.hpp src/Systems/RenderSystem.cpp
        src/Systems/ResourceDatabase.hpp src/Systems/ResourceDatabase.cpp
        src/Systems/TimeKeeper.hpp src/Systems/TimeKeeper.cpp

        # OGL
        src/OGL/Graphics.hpp src/OGL/Graphics.cpp
        src/OGL/IBindable.hpp
        src/OGL/IndexBuffer.hpp src/OGL/IndexBuffer.cpp
        src/OGL/VertexArray.hpp src/OGL/VertexArray.cpp
        src/OGL/VertexAttributes.hpp src/OGL/VertexAttributes.cpp
        src/OGL/VertexBuffer.hpp src/OGL/VertexBuffer.cpp
        src/OGL/UniformBuffer.hpp src/OGL/UniformBuffer.cpp

        # Resources
        src/Resources/Mesh.hpp src/Resources/Mesh.cpp
        src/Resources/ModelLoader.hpp src/Resources/ModelLoader.cpp
        src/Resources/Shader.hpp src/Resources/Shader.cpp
        src/Resources/ShaderLoader.hpp src/Resources/ShaderLoader.cpp
        src/Resources/Material.hpp src/Resources/Material.cpp
        src/Resources/ShaderStructs.hpp src/Resources/ShaderStructs.cpp
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

# Resources
set(source_dir "${CMAKE_SOURCE_DIR}/res")
set(destination_dir "${CMAKE_BINARY_DIR}")
file(COPY ${source_dir} DESTINATION ${destination_dir})