#include <functional>
#include "Material.hpp"
#include "../OGL/Graphics.hpp"


Material::Material(const Shader &shader): _shader(shader)
{
    auto u = _shader.GetUniforms();
    for(const auto &[name, slot] : _shader.GetUniforms())
    {
        if(name.starts_with("_"))
            continue;

        _uniformValues.emplace(slot.location, slot.defaultValue);
    }

    for(const auto &[name, slot] : _shader.GetTextureSlots())
        _textureValues.emplace(slot.unit, nullptr);
}

const Shader &Material::GetShader() const
{
    return _shader;
}

void Material::ApplyValues() const
{
    for (const auto &[location, value] : _uniformValues)
    {
        std::visit([location](const auto& v)
        {
            if constexpr (std::is_same_v<decltype(v), const bool&>)
                Shader::SetBool(location, v);
            else if constexpr (std::is_same_v<decltype(v), const float&>)
                Shader::SetFloat(location, v);
            else if constexpr (std::is_same_v<decltype(v), const glm::vec2&>)
                Shader::SetVec2(location, v);
            else if constexpr (std::is_same_v<decltype(v), const glm::vec3&>)
                Shader::SetVec3(location, v);
            else if constexpr (std::is_same_v<decltype(v), const glm::vec4&>)
                Shader::SetVec4(location, v);
            else if constexpr (std::is_same_v<decltype(v), const glm::mat3&>)
                Shader::SetMat3(location, v);
            else if constexpr (std::is_same_v<decltype(v), const glm::mat4&>)
                Shader::SetMat4(location, v);
        }, value);
    }
}

void Material::BindTextures() const
{
    for (auto &[location, texture] : _textureValues)
    {
        if(texture)
            Graphics::Bind(*texture, location);
    }
}