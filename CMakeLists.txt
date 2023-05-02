cmake_minimum_required(VERSION 3.25)
project(OGL_Engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/Application.cpp
        src/Entity.hpp src/Entity.cpp
        src/Components/Component.hpp src/Components/Component.cpp src/Components/TransformComponent.hpp src/Components/TransformComponent.cpp
        src/Systems/TimeKeeper.hpp src/Systems/TimeKeeper.cpp src/Input.hpp src/Input.cpp)


    # Libraries
# OpenGL libraries
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