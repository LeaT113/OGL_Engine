#include "LightComponent.hpp"

#include "../Entity/Entity.hpp"
#include "../Systems/LightingSystem.hpp"


LightComponent::LightComponent(const Entity* owner, LightType type)
    : _lightType(type), _color(1), _direction(0, -1, 0)
{
    _owner = owner;
    _transform = owner->GetComponent<TransformComponent>();

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
    float spotA = 1 / (glm::cos(glm::radians(_spotInnerAngle/2)) - glm::cos(glm::radians(_spotOuterAngle/2)));
    float spotB = - glm::cos(glm::radians(_spotInnerAngle/2));

    return SpotLight(_color, spotA, _transform->Position(), spotB, _direction);
}
