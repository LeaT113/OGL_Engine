#include <utility>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"


Shader::Shader(unsigned int id, std::string name,
    std::unordered_map<std::string, UniformSlot> uniforms, std::unordered_map<std::string, TextureSlot> textures,
    Cull cull, bool depthWrite, DepthTest depthTest, Blending alphaBlend)
    : _shader(id), _uniforms(std::move(uniforms)), _textures(std::move(textures)),
    _cull(cull), _depthWrite(depthWrite), _depthTest(depthTest), _blending(alphaBlend), Resource(std::move(name))
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

/// \brief Get the location of a simple uniform or the binding slot of a sampler for the currently bound shader
/// \param name Name of uniform in shader
/// \return Location or slot of uniform
int Shader::GetUniformLocation(const std::string &name) const
{
    auto it = _uniforms.find(name);
    if(it != _uniforms.end())
        return it->second.location;

    auto it2 = _textures.find(name);
    if(it2 != _textures.end())
        return it2->second.unit;

    return -1;
}

int Shader::GetTextureUnit(const std::string& name) const
{
    auto it = _textures.find(name);
    if(it != _textures.end())
        return it->second.unit;

    return -1;
}

const std::unordered_map<std::string, Shader::UniformSlot>& Shader::GetUniforms() const
{
    return _uniforms;
}

const std::unordered_map<std::string, Shader::TextureSlot>& Shader::GetTextureSlots() const
{
    return _textures;
}

void Shader::SetBool(int location, bool value)
{
    glUniform1i(location, value ? 1 : 0);
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

void Shader::SetTransformations(const glm::mat4 &modelToWorld, const glm::mat4 &view, const glm::mat4 &projection) const
{
    SetMat4(GetUniformLocation("_ModelMatrix"), modelToWorld);
    SetMat4(GetUniformLocation("_InvModelMatrix"), glm::inverse(modelToWorld));
    SetMat4(GetUniformLocation("_ViewMatrix"), view);
    SetMat4(GetUniformLocation("_InvViewMatrix"), glm::inverse(view));
    SetMat4(GetUniformLocation("_ProjectionMatrix"), projection);
    SetMat4(GetUniformLocation("_InvProjectionMatrix"), inverse(projection));
    SetMat4(GetUniformLocation("_MVPMatrix"), projection * view * modelToWorld);
    SetMat3(GetUniformLocation("_ModelNormalMatrix"), glm::transpose(glm::inverse(glm::mat3(modelToWorld))));
}

void Shader::SetTexture(const std::string& name, unsigned int textureId) const
{
    auto it = _textures.find(name);
    if (it != _textures.end())
    {
        glActiveTexture(GL_TEXTURE0 + it->second.unit);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
}

void Shader::SetTextures(unsigned int sceneColor, unsigned int sceneDepth, unsigned int cubemap) const
{
    auto it = _textures.find("_SceneColorTex");
    if (it != _textures.end())
    {
        glActiveTexture(GL_TEXTURE0 + it->second.unit);
        glBindTexture(GL_TEXTURE_2D, sceneColor);
    }

    it = _textures.find("_SceneDepthTex");
    if (it != _textures.end())
    {
        glActiveTexture(GL_TEXTURE0 + it->second.unit);
        glBindTexture(GL_TEXTURE_2D, sceneDepth);
    }

    it = _textures.find("_CubemapTex");
    if (it != _textures.end())
    {
        glActiveTexture(GL_TEXTURE0 + it->second.unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    }
}

void Shader::BindTextureUnits() const
{
    for (const auto& [name, slot] : _textures)
    {
        glUniform1i(slot.location, slot.unit);
    }
}

void Shader::SetPipelineState() const
{
    if (_cull == Cull::Off)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(static_cast<GLenum>(_cull));
    }

    glDepthMask(_depthWrite ? GL_TRUE : GL_FALSE);

    if (_depthTest == DepthTest::Off)
    {
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(static_cast<GLenum>(_depthTest));
    }

    if (_blending == Blending::Off)
    {
        glDisable(GL_BLEND);
    }
    else
    {
        glEnable(GL_BLEND);

        switch (_blending)
        {
        case Blending::Alpha:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case Blending::Additive:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        }
    }
}

bool Shader::IsOpaque() const
{
    return _blending == Blending::Off;
}
