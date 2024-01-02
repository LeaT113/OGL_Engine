#ifndef OGL_ENGINE_SHADER_HPP
#define OGL_ENGINE_SHADER_HPP

#include <unordered_map>
#include <typeindex>
#include <glm/glm.hpp>
#include "Texture.hpp"
#include "../OGL/IBindable.hpp"


class Shader : public IBindable
{
public:
    struct Uniform {
        int location;
        std::type_index type;
    };

    struct TextureSlot {
        int binding;
        Texture::Type type;
    };

    Shader(unsigned int id, std::string name, std::unordered_map<std::string, Uniform> uniforms, std::unordered_map<std::string, TextureSlot> textures);
    ~Shader() override;

    void Replace(Shader&& other) noexcept;

    unsigned int GetBindID() const override;

    int GetUniformLocation(const std::string &name) const;

    std::unordered_map<std::string, Uniform> GetUniforms() const;
    std::unordered_map<std::string, TextureSlot> GetTextureSlots() const;

    static void SetFloat(int location, float value);
    static void SetVec2(int location, glm::vec2 value);
    static void SetVec3(int location, glm::vec3 value);
    static void SetVec4(int location, glm::vec4 value);
    static void SetMat3(int location, glm::mat3 value);
    static void SetMat4(int location, glm::mat4 value);

    void SetTransformations(const glm::mat4 &modelToWorld, const glm::mat4 &view, const glm::mat4 &projection) const;

private:
    unsigned int _shader;
    std::string _name;
    std::unordered_map<std::string, Uniform> _uniforms;
    std::unordered_map<std::string, TextureSlot> _textures;
};


#endif