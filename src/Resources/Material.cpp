#include <functional>
#include "Material.hpp"


Material::Material(const Shader &shader): _shader(shader)
{
    static const std::unordered_map<std::type_index, std::function<MaterialValue()>> uniformTypeToInstance =
            {
                {typeid(float), []() -> MaterialValue { return float(); }},
                {typeid(glm::vec2), []() -> MaterialValue { return glm::vec2(); }},
                {typeid(glm::vec3), []() -> MaterialValue { return glm::vec3(); }},
                {typeid(glm::vec4), []() -> MaterialValue { return glm::vec4(); }},
                {typeid(glm::mat3), []() -> MaterialValue { return glm::mat3(); }},
                {typeid(glm::mat4), []() -> MaterialValue { return glm::mat4(); }},
            };

    for(const auto &u : _shader.GetUniforms())
        _uniformLocationValue.emplace(u.second.location, uniformTypeToInstance.at(u.second.type)());
}

const Shader &Material::GetShader() const
{
    return _shader;
}

void Material::ApplyValues() const
{
    for (const auto &[location, value] : _uniformLocationValue)
    {
        std::visit([location](const auto& v)
        {
            if constexpr (std::is_same_v<decltype(v), const float&>)
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
