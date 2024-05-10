#ifndef LIGHTCOMPONENT_HPP
#define LIGHTCOMPONENT_HPP

#include "Component.hpp"
#include "../OGL/ShaderStructs.hpp"

class TransformComponent;

class LightComponent : public Component
{
public:
    enum class Type {
        Ambient,
        Direct,
        Point,
        Spot
    };

    LightComponent(const Entity& owner, Type type);

    glm::vec3 GetColor() const;

    void SetColor(const glm::vec3 &color);
    void SetDirection(const glm::vec3 &direction);
    void SetSpotAngles(float innerAngle, float outerAngle);
    std::pair<float, float> GetSpotAngles() const;

    void SetShadowCasting(bool enable);
    bool IsShadowCasting() const;
    void SetShadowIndex(int index);

    Type GetType() const;
    AmbientLight GetAmbientLight() const;
    DirectLight GetDirectLight() const;
    PointLight GetPointLight() const;
    SpotLight GetSpotLight() const;

private:
    friend class Serializer;

    Type _lightType = Type::Ambient;
    glm::vec3 _color = glm::vec3(0);
    glm::vec3 _direction = glm::vec3(0, 0, -1);
    float _spotInnerAngle = 0;
    float _spotOuterAngle = 0;

    bool _shadowCasting = false;
    int _shadowMapIndex = -1;
};



#endif
