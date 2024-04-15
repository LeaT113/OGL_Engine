#include "Component.hpp"

#include "../Scene/Entity.hpp"

Component::Component(const Entity& owner) : _owner(&owner)
{}

Component::~Component() = default;

TransformComponent* Component::GetTransform() const
{
    return _owner->GetTransform();
}
