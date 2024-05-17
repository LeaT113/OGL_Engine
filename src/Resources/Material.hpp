#ifndef OGL_ENGINE_MATERIAL_HPP
#define OGL_ENGINE_MATERIAL_HPP

#include "Resource.hpp"
#include "Shader.hpp"

class Texture;


class Material : public Resource
{
public:
    explicit Material(const Shader &shader, std::string name);

    template<typename T>
    void Set(int location, const T& value)
    {
        auto it = _uniformValues.find(location);
        if (it == _uniformValues.end())
            return;// TODO throw std::runtime_error("Material::Set invalid property location");

        if (!std::holds_alternative<T>(it->second))
            return;// TODO throw std::runtime_error("Material::Set type mismatch");

        it->second = value;
    }

    void Set(int location, Texture* value)
    {
        if (location < 0 || location >= _textureValues.size())
            return;// TODO throw std::runtime_error("Material::Set invalid texture slot");

        _textureValues[location] = value;
    }

    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        Set(_shader.GetUniformLocation(name), value);
    }

    // TODO Specialize get for textures
    template<typename T>
    T Get(const std::string& name) const
    {
        return Get<T>(_shader.GetUniformLocation(name));
    }

    template<typename T>
    T Get(int location) const
    {
        auto it = _uniformValues.find(location);
        if (it == _uniformValues.end())
            throw std::runtime_error("Material::Set invalid property location");

        const auto &property = it->second;
        if (!std::holds_alternative<T>(property))
            throw std::runtime_error("Material::Set type mismatch");

        return std::get<T>(property);
    }

    void ApplyValues() const;

    void BindTextures() const;

    const Shader &GetShader() const;

private:
    friend class MaterialLoader;

    const Shader &_shader;

    std::unordered_map<int, Shader::UniformValue> _uniformValues;
    std::unordered_map<int, Texture*> _textureValues;
};


#endif
