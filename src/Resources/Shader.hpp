#ifndef OGL_ENGINE_SHADER_HPP
#define OGL_ENGINE_SHADER_HPP

#include <unordered_map>
#include <typeindex>
#include <variant>
#include <glm/glm.hpp>

#include "Resource.hpp"
#include "Texture.hpp"
#include "../OGL/IBindable.hpp"


class Shader : public Resource, public IBindable
{
public:
    using UniformValue = std::variant<bool, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

    enum class Cull
    {
        Off = 0,
        Back = GL_BACK,
        Front = GL_FRONT,

        Default = Back
    };

    enum class DepthTest
    {
        Off = GL_ALWAYS,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        LEqual = GL_LEQUAL,
        GEqual = GL_GEQUAL,
        Greater = GL_GREATER,

        Default = LEqual,
    };

    enum class Blending
    {
        Off,
        Alpha,
        Additive
    };

    struct UniformSlot
    {
        int location;
        std::type_index type;
        UniformValue defaultValue;
    };

    struct TextureSlot
    {
        int location;
        int unit;
        Texture::Type type;
    };


    Shader(unsigned int id, std::string name,
        std::unordered_map<std::string, UniformSlot> uniforms, std::unordered_map<std::string, TextureSlot> textures,
        Cull cull, bool depthWrite, DepthTest depthTest, Blending alphaBlend);
    ~Shader() override;

    void Replace(Shader&& other) noexcept;

    unsigned int GetBindID() const override;

    int GetUniformLocation(const std::string &name) const;

    const std::unordered_map<std::string, UniformSlot>& GetUniforms() const;
    const std::unordered_map<std::string, TextureSlot>& GetTextureSlots() const;

    static void SetBool(int location, bool value);
    static void SetFloat(int location, float value);
    static void SetVec2(int location, glm::vec2 value);
    static void SetVec3(int location, glm::vec3 value);
    static void SetVec4(int location, glm::vec4 value);
    static void SetMat3(int location, glm::mat3 value);
    static void SetMat4(int location, glm::mat4 value);

    void SetTransformations(const glm::mat4 &modelToWorld, const glm::mat4 &view, const glm::mat4 &projection) const;

    void BindTextureUnits() const;

    void SetPipelineState() const;

    bool IsOpaque() const;

private:
    unsigned int _shader;
    std::unordered_map<std::string, UniformSlot> _uniforms;
    std::unordered_map<std::string, TextureSlot> _textures;

    // Pipeline states
    Cull _cull;
    bool _depthWrite;
    DepthTest _depthTest;
    Blending _blending;
};


#endif