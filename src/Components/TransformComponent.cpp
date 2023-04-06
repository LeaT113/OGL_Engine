#include "TransformComponent.hpp"
#include <functional>


TransformComponent::TransformComponent(): _position(0), _rotation(), _scale(1)
{}

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

void TransformComponent::AngleAxis(float angle, const glm::vec3 &axis)
{
	_rotation = glm::angleAxis(glm::radians(angle),  glm::normalize(axis)) * _rotation;
}

void TransformComponent::AngleAxisPoint(float angle, const glm::vec3 &axis, const glm::vec3 &point) // TODO Check correctness
{
	auto rot = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
	_rotation = rot * _rotation;
	_position = rot * _position;
}
