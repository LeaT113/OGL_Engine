cmake_minimum_required(VERSION 3.25)
project(OGL_Engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        src/Application.cpp

        src/Common/Singleton.hpp src/Common/HybridSingleton.hpp

        src/Components/Component.hpp src/Components/Component.cpp src/Components/TransformComponent.hpp
        src/Components/TransformComponent.cpp src/Components/CameraComponent.hpp src/Components/CameraComponent.cpp
        src/Components/RendererComponent.hpp src/Components/RendererComponent.cpp

        src/Data/MeshData.hpp src/Data/MeshData.cpp src/Data/MaterialData.hpp src/Data/MaterialData.cpp
        src/Data/ShaderData.hpp src/Data/ShaderData.cpp

        src/Entity/Entity.hpp src/Entity/Entity.cpp

        src/Systems/TimeKeeper.hpp src/Systems/TimeKeeper.cpp src/Systems/InputSystem.hpp src/Systems/InputSystem.cpp
        src/Systems/ResourceDatabase.hpp src/Systems/ResourceDatabase.cpp src/Systems/RenderSystem.hpp
        src/Systems/RenderSystem.cpp)

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