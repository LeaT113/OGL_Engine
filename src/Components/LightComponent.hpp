#ifndef LIGHTCOMPONENT_HPP
#define LIGHTCOMPONENT_HPP

#include "Component.hpp"
#include "TransformComponent.hpp"
#include "../Resources/ShaderStructs.hpp"


class LightComponent : public Component
{
public:
    enum LightType {
        Ambient,
        Direct,
        Point,
        Spot
    };

    explicit LightComponent(const Entity *owner, LightType type);

    void SetColor(const glm::vec3 &color);
    void SetDirection(const glm::vec3 &direction);
    void SetSpotAngles(float innerAngle, float outerAngle);

    LightType GetType() const;
    AmbientLight GetAmbientLight() const;
    DirectLight GetDirectLight() const;
    PointLight GetPointLight() const;
    SpotLight GetSpotLight() const;

private:
    TransformComponent *_transform;

    LightType _lightType;
    glm::vec3 _color;
    glm::vec3 _direction;
    float _spotInnerAngle;
    float _spotOuterAngle;
};



#endif
