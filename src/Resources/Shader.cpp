#include <stdexcept>
#include <utility>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "../OGL/Graphics.hpp"


Shader::Shader(unsigned int id, std::string name): _shader(id), _name(std::move(name))
{
    GLint uniformCount = 0;
    glGetProgramiv(_shader, GL_ACTIVE_UNIFORMS, &uniformCount);
    if(uniformCount == 0)
        return;

    GLint 	nameMaxLength = 0;
    GLsizei length = 0;
    GLsizei count = 0;
    GLenum 	type = GL_NONE;
    glGetProgramiv(_shader, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameMaxLength);

    auto uniformName = std::make_unique<char[]>(nameMaxLength);
    for (GLint i = 0; i < uniformCount; ++i)
    {
        glGetActiveUniform(_shader, i, nameMaxLength, &length, &count, &type, uniformName.get());

        std::string nameString (uniformName.get(), length);
        int location = glGetUniformLocation(_shader, uniformName.get());
        std::type_index dataType = TypeOpenglToCpp(type);

        _uniformsNameLocationType.emplace(std::move(nameString), std::make_pair(location, dataType));
    }
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
    _uniformsNameLocationType = std::move(other._uniformsNameLocationType);

    other._shader = -1;
}

unsigned int Shader::GetBindID() const
{
    return _shader;
}

int Shader::GetUniformLocation(const std::string &name) const
{
    auto it = _uniformsNameLocationType.find(name);
    if(it == _uniformsNameLocationType.end())
        return -1;

    return it->second.first;
}

std::unordered_map<std::string, std::pair<int, std::type_index>> Shader::GetUniforms() const
{
    return _uniformsNameLocationType;
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

std::type_index Shader::TypeOpenglToCpp(unsigned int type)
{
    switch(type)
    {
        case GL_FLOAT:
            return typeid(float);

        case GL_FLOAT_VEC2:
            return typeid(glm::vec2);
        case GL_FLOAT_VEC3:
            return typeid(glm::vec3);
        case GL_FLOAT_VEC4:
            return typeid(glm::vec4);

        case GL_FLOAT_MAT3:
            return typeid(glm::mat3);
        case GL_FLOAT_MAT4:
            return typeid(glm::mat4);

        default:
            throw std::runtime_error("Shader::TypeOpenglToCpp unsupported type");
    }
}

void Shader::SetTransformations(const glm::mat4 &modelToWorld, const CameraComponent &camera) const
{
    SetMat4(GetUniformLocation("ObjectToWorldMatrix"), modelToWorld);
    SetMat4(GetUniformLocation("WorldToObjectMatrix"), glm::inverse(modelToWorld));
    SetMat4(GetUniformLocation("WorldToViewMatrix"), camera.View());
    SetMat4(GetUniformLocation("ViewToWorldMatrix"), glm::inverse(camera.View()));

    SetMat4(GetUniformLocation("ObjectToClipMatrix"), camera.Projection() * camera.View() * modelToWorld);
}
