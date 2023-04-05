#include "TransformComponent.hpp"
#include <functional>


TransformComponent::TransformComponent() = default;

const glm::vec3 &TransformComponent::Position() const
{
	return std::cref(_position);
}

glm::vec3 &TransformComponent::Position()
{
	_changed = true;
	return _position;
}

const glm::quat &TransformComponent::Rotation() const
{
	return std::cref(_rotation);
}

glm::quat &TransformComponent::Rotation()
{
	_changed = true;
	return _rotation;
}

const glm::vec3 &TransformComponent::Scale() const
{
	return std::cref(_scale);
}

glm::vec3 &TransformComponent::Scale()
{
	_changed = true;
	return _scale;
}
