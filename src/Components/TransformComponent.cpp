#include "TransformComponent.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

TransformComponent::TransformComponent(const Entity& owner)
	: Component(owner)
{}

glm::vec3 TransformComponent::Position() const
{
	if (_parent)
		return _parent->ModelToWorld() * glm::vec4(_position, 1);
	return _position;
}

void TransformComponent::Position(const glm::vec3 &position)
{
	if (_parent)
		_position = _parent->WorldToModel() * glm::vec4(position, 1);
	else
		_position = position;

	_valid = false;
}

glm::quat TransformComponent::Rotation() const
{
	if (_parent)
		return _rotation * _parent->Rotation();
	return _rotation;
}

void TransformComponent::Rotation(const glm::quat &rotation)
{
	if (_parent)
		_rotation = inverse(_parent->Rotation()) * rotation;
	else
		_rotation = rotation;

	_valid = false;
}

glm::vec3 TransformComponent::LocalScale() const
{
	return _scale;
}

void TransformComponent::LocalScale(const glm::vec3 &scale)
{
	_scale = scale;
	_valid = false;
}

glm::vec3 TransformComponent::LocalPosition() const
{
	return _position;
}

void TransformComponent::LocalPosition(const glm::vec3& position)
{
	_position = position;
	_valid = false;
}

glm::quat TransformComponent::LocalRotation() const
{
	return _rotation;
}

void TransformComponent::LocalRotation(const glm::quat& rotation)
{
	_rotation = rotation;
	_valid = false;
}

void TransformComponent::SetParent(TransformComponent* parent)
{
	if (parent == _parent)
		return;

	if (_parent)
	{
		_position = _parent->ModelToWorld() * glm::vec4(_position, 1);
		_rotation = _parent->Rotation() * _rotation;
	}
	if (parent)
	{
		_position = parent->WorldToModel() * glm::vec4(_position, 1);
		_rotation = inverse(parent->Rotation()) * _rotation;
	}
	_parent = parent;
	_valid = false;
}

TransformComponent* TransformComponent::GetParent() const
{
	return _parent;
}

glm::mat4 TransformComponent::ModelToWorld() const
{
	if(!_valid)
		Update();

	if (_parent)
		return _parent->ModelToWorld() * _modelToWorld;
	return _modelToWorld;
}

glm::mat4 TransformComponent::WorldToModel() const
{
	if(!_valid)
		Update();

	if (_parent)
		return _worldToModel * _parent->WorldToModel();
	return _worldToModel;
}


void TransformComponent::AngleAxis(float angle, const glm::vec3 &axis)
{
	Rotation(angleAxis(glm::radians(angle), normalize(axis)) * Rotation());
}

void TransformComponent::AngleAxisPoint(float angle, const glm::vec3 &axis, const glm::vec3 &point) // TODO Check correctness
{
	auto rot = angleAxis(glm::radians(angle), normalize(axis));
	Rotation(rot * _rotation);
	Position(rot * _position);
}

void TransformComponent::LookAt(glm::vec3 direction, glm::vec3 up)
{
	Rotation(quatLookAt(direction, up));
}

void TransformComponent::AlignWith(const TransformComponent& other)
{
	Position(other.Position());
	Rotation(other.Rotation());
}

void TransformComponent::Update() const
{
	_modelToWorld = translate(_position) * toMat4(_rotation) * scale(_scale);
	_worldToModel = inverse(_modelToWorld);
	_valid = true;
}

glm::vec3 TransformComponent::Up() const
{
	return ModelToWorld() * glm::vec4(0, 1, 0, 0);
}

glm::vec3 TransformComponent::Forward() const
{
	return ModelToWorld() * glm::vec4(0, 0, -1, 0);
}

glm::vec3 TransformComponent::Right() const
{
	return ModelToWorld() * glm::vec4(1, 0, 0, 0);
}
