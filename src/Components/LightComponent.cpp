#include "LightComponent.hpp"

#include "../Scene/Entity.hpp"
#include "../Systems/LightingSystem.hpp"
#include "TransformComponent.hpp"


LightComponent::LightComponent(const Entity& owner, LightType type)
    : Component(owner), _lightType(type), _color(1), _direction(0, -1, 0), _spotInnerAngle(20), _spotOuterAngle(30)
{
    LightingSystem::RegisterLight(this);
}

LightComponent::LightComponent(const Entity& owner) : Component(owner)
{
    LightingSystem::RegisterLight(this);
}


void LightComponent::SetColor(const glm::vec3& color)
{
    _color = color;
}

void LightComponent::SetDirection(const glm::vec3& direction)
{
    _direction = glm::normalize(direction);
}

void LightComponent::SetSpotAngles(float innerAngle, float outerAngle)
{
    _spotInnerAngle = innerAngle;
    _spotOuterAngle = outerAngle;
}

LightComponent::LightType LightComponent::GetType() const
{
    return _lightType;
}

AmbientLight LightComponent::GetAmbientLight() const
{
    return AmbientLight(_color);
}

DirectLight LightComponent::GetDirectLight() const
{
    return DirectLight(_color, 0, _direction);
}

PointLight LightComponent::GetPointLight() const
{
    return PointLight(_color, 0, _transform->Position());
}

SpotLight LightComponent::GetSpotLight() const
{
    float innerCos = glm::cos(glm::radians(_spotInnerAngle * 0.5f));
    float outerCos = glm::cos(glm::radians(_spotOuterAngle * 0.5f));

    float a = 1 / glm::max(innerCos - outerCos, 0.001f);
    float b = -outerCos * a;

    //float spotA = 1 / (glm::cos(glm::radians(_spotInnerAngle/2)) - glm::cos(glm::radians(_spotOuterAngle/2)));
    //float spotB = - glm::cos(glm::radians(_spotInnerAngle/2));

    glm::vec3 dirWS = glm::normalize(GetTransform()->ModelToWorld() * glm::vec4(_direction, 0));

    return SpotLight(_color, a, GetTransform()->Position(), b, dirWS);
}
