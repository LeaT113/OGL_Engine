#ifndef OGL_ENGINE_SHADER_HPP
#define OGL_ENGINE_SHADER_HPP

#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/fwd.hpp>
#include <typeindex>
#include <vector>
#include "../OGL/IBindable.hpp"
#include "../Components/CameraComponent.hpp"


class Shader : public IBindable
{
public:
    explicit Shader(unsigned int id, std::string name);
    ~Shader();

    void Replace(Shader&& other) noexcept;

    unsigned int GetBindID() const override;

    int GetUniformLocation(const std::string &name) const;

    std::unordered_map<std::string, std::pair<int, std::type_index>> GetUniforms() const;

    static void SetFloat(int location, float value);
    static void SetVec2(int location, glm::vec2 value);
    static void SetVec3(int location, glm::vec3 value);
    static void SetVec4(int location, glm::vec4 value);
    static void SetMat3(int location, glm::mat3 value);
    static void SetMat4(int location, glm::mat4 value);

    void SetTransformations(glm::mat4 modelToWorld, CameraComponent &camera) const;

private:
    unsigned int _shader;
    std::string _name;
    std::unordered_map<std::string, std::pair<int, std::type_index>> _uniformsNameLocationType;

    static std::type_index TypeOpenglToCpp(unsigned int type);
};


#endif