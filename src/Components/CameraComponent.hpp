#ifndef OGL_ENGINE_CAMERACOMPONENT_HPP
#define OGL_ENGINE_CAMERACOMPONENT_HPP

#include <glm/glm.hpp>
#include "Component.hpp"
#include "TransformComponent.hpp"

class Entity;

class CameraComponent : public Component
{
public:
	enum class ProjectionType
	{
		Orthographic,
		Perspective
	};

	CameraComponent(const Entity& owner, ProjectionType projectionType, float horizontalFov);
	explicit CameraComponent(const Entity& owner);

	void SetWindowDimensions(int width, int height);
	void SetFOV(float horizontalFov);

	// Matrices
	const glm::mat4 &View() const;
	const glm::mat4 &Projection() const;

	glm::vec2 GetNearFar() const;

private:
	friend class Serializer;

	static constexpr float NearClippingPlane = 0.1f;
	static constexpr float FarClippingPlane = 1000.0f;


	// Data
	ProjectionType _projectionType;
	float _horizontalFov;
	int _width, _height;

	mutable glm::mat4 _projectionMatrix;
	mutable bool _projectionValid;
};


#endif
