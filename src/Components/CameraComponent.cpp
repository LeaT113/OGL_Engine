#include "CameraComponent.hpp"
#include "../Entity/Entity.hpp"

CameraComponent::CameraComponent(const Entity *owner, ProjectionType projectionType, float horizontalFov)
		: _projectionType(projectionType), _horizontalFov(horizontalFov), _projectionValid(false)
{
	_owner = owner;

	_transform = _owner->GetComponent<TransformComponent>();
	if (_transform == nullptr)
		throw std::runtime_error("Entity must have Transform for Camera to work");

	SetWindowDimensions(1, 1);
}

void CameraComponent::SetWindowDimensions(int width, int height)
{
	_width = width;
	_height = height;

	_projectionValid = false;
}

const glm::mat4 &CameraComponent::View() const
{
	return _transform->WorldToModel();
}

const glm::mat4 &CameraComponent::Projection() const
{
	if(!_projectionValid)
	{
		if(_projectionType == ProjectionType::Orthographic)
		{
			float orthoScale = _horizontalFov / 90.0f;
			_projectionMatrix = glm::ortho(
					-(float)_width / 2 * orthoScale,
					(float)_width / 2 * orthoScale,
					-(float)_height / 2 * orthoScale,
					(float)_height / 2 * orthoScale,
					NearClippingPlane,
					FarClippingPlane);
		}
		else
		{
			float aspectRatio = (float)_width / (float)_height;
			float vFov = 2 * atan(tan(glm::radians(_horizontalFov) / 2) * aspectRatio);

			_projectionMatrix = glm::perspective(
					vFov,
					aspectRatio,
					NearClippingPlane,
					FarClippingPlane);
		}

		_projectionValid = true;
	}

	return _projectionMatrix;
}



