#ifndef OGL_ENGINE_CAMERACOMPONENT_HPP
#define OGL_ENGINE_CAMERACOMPONENT_HPP

#include <glm/glm.hpp>
#include "Component.hpp"
#include "TransformComponent.hpp"

class Entity;

enum ProjectionType
{
	Orthographic,
	Perspective
};

class CameraComponent : public Component
{
public:
	CameraComponent(const Entity* owner, ProjectionType projectionType, float horizontalFov);

	void SetWindowDimensions(int width, int height);

	// Matrices
	const glm::mat4 &View() const;
	const glm::mat4 &Projection() const;

private:
	static constexpr float NearClippingPlane = 0.1f;
	static constexpr float FarClippingPlane = 1000.0f;


	// Data
	ProjectionType _projectionType;
	float _horizontalFov;
	int _width, _height;

	mutable glm::mat4 _projectionMatrix;
	mutable bool _projectionValid;

	TransformComponent* _transform;
};


#endif
