#include "Transform.hpp"
#include <functional>


const glm::vec3 &Transform::Position() const
{
	return std::cref(_position);
}

glm::vec3 &Transform::Position()
{
	_changed = true;
	return _position;
}

const glm::quat &Transform::Rotation() const
{
	return std::cref(_rotation);
}

glm::quat &Transform::Rotation()
{
	_changed = true;
	return _rotation;
}

const glm::vec3 &Transform::Scale() const
{
	return std::cref(_scale);
}

glm::vec3 &Transform::Scale()
{
	_changed = true;
	return _scale;
}
