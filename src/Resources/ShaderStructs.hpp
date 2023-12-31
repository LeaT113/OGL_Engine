#ifndef SHADERSTRUCTS_HPP
#define SHADERSTRUCTS_HPP
#include <glm/vec3.hpp>

struct AmbientLight
{
    glm::vec3 color;
    float _padding0;
};

struct DirectLight
{
    glm::vec3 color;
    float _padding0;
    glm::vec3 direction;
    float _padding1;
};

struct PointLight
{
    glm::vec3 color;
    float _padding0;
    glm::vec3 position;
    float radius;
};

struct SpotLight
{
    glm::vec3 color;
    float a;
    glm::vec3 position;
    float b;
    glm::vec3 direction;
    float _padding0;
};


#endif
