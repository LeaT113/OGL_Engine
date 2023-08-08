#include "TransformComponent.hpp"
#include <functional>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <iomanip>


TransformComponent::TransformComponent(const Entity* owner): _position(0), _rotation(glm::vec3(0, 0, 0)), _scale(1)
{
	_owner = owner;
}

const glm::vec3 &TransformComponent::Position() const
{
	return std::cref(_position);
}

glm::vec3 &TransformComponent::Position()
{
	NotifyChanged();
	return _position;
}

const glm::quat &TransformComponent::Rotation() const
{
	return std::cref(_rotation);
}

glm::quat &TransformComponent::Rotation()
{
	NotifyChanged();
	return _rotation;
}

const glm::vec3 &TransformComponent::Scale() const
{
	return std::cref(_scale);
}

glm::vec3 &TransformComponent::Scale()
{
	NotifyChanged();
	return _scale;
}

const glm::mat4 &TransformComponent::ModelToWorld()
{
	bool valid = _validMatrixBits & 1;
	if(!valid)
	{
		_modelToWorld = glm::translate(_position) * glm::toMat4(_rotation) * glm::scale(_scale);
		_validMatrixBits |= 1;
	}

	return _modelToWorld;
}

const glm::mat4 &TransformComponent::WorldToModel()
{
	bool valid = _validMatrixBits & 2;
	if(!valid)
	{
		_worldToModel = glm::inverse(ModelToWorld());
		_validMatrixBits |= 2;
	}

	return _worldToModel;
}


void TransformComponent::NotifyChanged()
{
	_validMatrixBits = 0;
}

void TransformComponent::AngleAxis(float angle, const glm::vec3 &axis)
{
	Rotation() = glm::angleAxis(glm::radians(angle),  glm::normalize(axis)) * _rotation;
}

void TransformComponent::AngleAxisPoint(float angle, const glm::vec3 &axis, const glm::vec3 &point) // TODO Check correctness
{
	auto rot = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
	Rotation() = rot * _rotation;
	Position() = rot * _position;
}

glm::vec3 TransformComponent::Up()
{
	return ModelToWorld() * glm::vec4(0, 1, 0, 0);
}

glm::vec3 TransformComponent::Forward()
{
	return ModelToWorld() * glm::vec4(0, 0, -1, 0);
}

glm::vec3 TransformComponent::Right()
{
	return ModelToWorld() * glm::vec4(1, 0, 0, 0);
}


std::ostream& operator<<(std::ostream& os, const TransformComponent& transform)
{
    constexpr int floatLength = 6;
    auto eulerRotation = glm::eulerAngles(transform.Rotation());
    os
            << "----- Transform -----"
            << "\nPos | " << std::setw(floatLength) << transform.Position().x << " | " << std::setw(floatLength) << transform.Position().y << " | " << std::setw(floatLength) << transform.Position().z << " |"
            << "\nRot | " << std::setw(floatLength) << glm::degrees(eulerRotation.x) << " | " << std::setw(floatLength) << glm::degrees(eulerRotation.y) << " | " << std::setw(floatLength) << glm::degrees(eulerRotation.z) << " |"
            << "\nScl | " << std::setw(floatLength) << transform.Scale().x << " | " << std::setw(floatLength) << transform.Scale().y << " | " << std::setw(floatLength) << transform.Scale().z << " |";

    return os;
}