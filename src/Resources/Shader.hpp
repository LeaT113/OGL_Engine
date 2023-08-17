#ifndef OGL_ENGINE_SHADER_HPP
#define OGL_ENGINE_SHADER_HPP

#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/fwd.hpp>
#include "../OGL/IBindable.hpp"


class Shader : public IBindable
{
public:
    explicit Shader(unsigned int id, std::string name);
    ~Shader();

    void Replace(Shader&& other) noexcept;

    unsigned int GetBindID() const override;

    // Uniforms
    int GetUniformLocation(const std::string &name) const;

    static void SetFloat(int location, float value);
    static void SetVec2(int location, glm::vec2 value);
    static void SetVec3(int location, glm::vec3 value);
    static void SetVec4(int location, glm::vec4 value);
    static void SetMat3(int location, glm::mat3 value);
    static void SetMat4(int location, glm::mat4 value);

private:
    unsigned int _shader;
    std::string _name;
    std::unordered_map<std::string, int> _uniformLocations;
};


#endif