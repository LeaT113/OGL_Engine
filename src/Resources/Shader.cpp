#include <stdexcept>
#include <utility>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"


Shader::Shader(unsigned int id, std::string name): _shader(id), _name(std::move(name))
{
}

Shader::~Shader()
{
    glDeleteProgram(_shader);
}

void Shader::Replace(Shader &&other) noexcept
{
    if(this == &other)
        return;

    glDeleteProgram(_shader);

    _shader = other._shader;
    _uniformLocations = std::move(other._uniformLocations);

    other._shader = -1;
}

unsigned int Shader::GetBindID() const
{
    return _shader;
}

int Shader::GetUniformLocation(const std::string &name) const
{
    auto it = _uniformLocations.find(name);
    if(it == _uniformLocations.end())
        return -1;

    return it->second;
}

void Shader::SetFloat(int location, float value) const
{
    glUniform1f(location, value);
}

void Shader::SetVec2(int location, glm::vec2 value) const
{
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(int location, glm::vec3 value) const
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec4(int location, glm::vec4 value) const
{
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::SetMat3(int location, glm::mat3 value) const
{
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(int location, glm::mat4 value) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
