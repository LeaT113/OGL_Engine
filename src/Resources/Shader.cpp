#include <stdexcept>
#include <utility>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "../OGL/Graphics.hpp"


Shader::Shader(unsigned int id, std::string name, std::unordered_map<std::string, Uniform> uniforms)
    : _shader(id), _name(std::move(name)), _uniforms(std::move(uniforms))
{}

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
    _uniforms = std::move(other._uniforms);

    other._shader = -1;
}

unsigned int Shader::GetBindID() const
{
    return _shader;
}

int Shader::GetUniformLocation(const std::string &name) const
{
    auto it = _uniforms.find(name);
    if(it == _uniforms.end())
        return -1;

    return it->second.location;
}

std::unordered_map<std::string, Shader::Uniform> Shader::GetUniforms() const
{
    return _uniforms;
}

void Shader::SetFloat(int location, float value)
{
    glUniform1f(location, value);
}

void Shader::SetVec2(int location, glm::vec2 value)
{
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(int location, glm::vec3 value)
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec4(int location, glm::vec4 value)
{
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::SetMat3(int location, glm::mat3 value)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(int location, glm::mat4 value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetTransformations(const glm::mat4 &modelToWorld, const CameraComponent &camera) const
{
    SetMat4(GetUniformLocation("ObjectToWorldMatrix"), modelToWorld);
    SetMat4(GetUniformLocation("WorldToObjectMatrix"), glm::inverse(modelToWorld));
    SetMat4(GetUniformLocation("WorldToViewMatrix"), camera.View());
    SetMat4(GetUniformLocation("ViewToWorldMatrix"), glm::inverse(camera.View()));

    SetMat4(GetUniformLocation("ObjectToClipMatrix"), camera.Projection() * camera.View() * modelToWorld);
}
