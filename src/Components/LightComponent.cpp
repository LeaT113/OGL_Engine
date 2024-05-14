#include "LightComponent.hpp"

#include "../Scene/Entity.hpp"
#include "../Systems/LightingSystem.hpp"
#include "TransformComponent.hpp"


LightComponent::LightComponent(const Entity& owner, Type type)
    : Component(owner), _lightType(type)
{
    LightingSystem::RegisterLight(this);
}

glm::vec3 LightComponent::GetColor() const
{
    return _color;
}

void LightComponent::SetColor(const glm::vec3& color)
{
    _color = color;
}

glm::vec3 LightComponent::GetDirection() const
{
    return _direction;
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

std::pair<float, float> LightComponent::GetSpotAngles() const
{
    return std::make_pair(_spotInnerAngle, _spotOuterAngle);
}

void LightComponent::SetShadowCasting(bool enable)
{
    if (_lightType == Type::Ambient)
        throw std::runtime_error("LightComponent Shadows not supported for ambient light");

    _shadowCasting = enable;
    if (!enable)
        SetShadowIndex(-1);
}

LightComponent::Type LightComponent::GetType() const
{
    return _lightType;
}

bool LightComponent::IsShadowCasting() const
{
    return _shadowCasting;
}

void LightComponent::SetShadowIndex(int index)
{
    _shadowMapIndex = index;
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
    return PointLight(_color, _shadowMapIndex, GetTransform()->Position());
}

SpotLight LightComponent::GetSpotLight() const
{
    float innerCos = glm::cos(glm::radians(_spotInnerAngle * 0.5f));
    float outerCos = glm::cos(glm::radians(_spotOuterAngle * 0.5f));

    float a = 1 / glm::max(innerCos - outerCos, 0.001f);
    float b = -outerCos * a;

    glm::vec3 dirWS = glm::normalize(GetTransform()->ModelToWorld() * glm::vec4(_direction, 0));

    return SpotLight(_color, a, GetTransform()->Position(), b, dirWS, _shadowMapIndex);
}
