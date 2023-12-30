#ifndef OGL_ENGINE_MATERIAL_HPP
#define OGL_ENGINE_MATERIAL_HPP

#include <string>
#include <variant>
#include <stdexcept>
#include <glm/glm.hpp>
#include "Shader.hpp"


class Material
{
public:
    explicit Material(const Shader &shader);

    template<typename T>
    void Set(int location, const T& value)
    {
        auto it = _uniformLocationValue.find(location);
        if (it == _uniformLocationValue.end())
            throw std::runtime_error("Material::Set invalid property location");

        if (!std::holds_alternative<T>(it->second))
            throw std::runtime_error("Material::Set type mismatch");

        it->second = value;
    }

    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        Set(_shader.GetUniformLocation(name), value);
    }

    template<typename T>
    T Get(const std::string& name) const
    {
        return Get<T>(_shader.GetUniformLocation(name));
    }

    template<typename T>
    T Get(int location) const
    {
        auto it = _uniformLocationValue.find(location);
        if (it == _uniformLocationValue.end())
            throw std::runtime_error("Material::Set invalid property location");

        const MaterialValue &property = it->second;
        if (!std::holds_alternative<T>(property))
            throw std::runtime_error("Material::Set type mismatch");

        return std::get<T>(property);
    }

    void ApplyValues() const;

    const Shader &GetShader() const;

private:
    const Shader &_shader;
    using MaterialValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;
    std::unordered_map<int, MaterialValue> _uniformLocationValue;
};


#endif
