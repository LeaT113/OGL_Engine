#ifndef SHADERSTRUCTS_HPP
#define SHADERSTRUCTS_HPP
#include <glm/vec3.hpp>

struct PointLight
{
    glm::vec3 position;
    float _padding0;
    glm::vec3 color;

    float radius;
};


#endif
