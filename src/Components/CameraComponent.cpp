#include "CameraComponent.hpp"
#include "../Scene/Entity.hpp"

CameraComponent::CameraComponent(const Entity &owner, ProjectionType projectionType, float horizontalFov)
		: Component(owner), _projectionType(projectionType), _horizontalFov(horizontalFov), _projectionValid(false)
{
	SetWindowDimensions(1, 1);
}

CameraComponent::CameraComponent(const Entity& owner) : Component(owner)
{
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
	return GetTransform()->WorldToModel();
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

glm::vec2 CameraComponent::GetNearFar() const
{
	return glm::vec2(NearClippingPlane, FarClippingPlane);
}
