#ifndef OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#define OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#include "Component.hpp"

class TransformComponent : public Component
{
public:
	explicit TransformComponent(const Entity& owner);

	// Global positioning
	glm::vec3 Position() const;
	void Position(const glm::vec3 &position);
	glm::quat Rotation() const;
	void Rotation(const glm::quat &rotation);

	// Local positioning
	glm::vec3 LocalPosition() const;
	void LocalPosition(const glm::vec3 &position);
	glm::quat LocalRotation() const;
	void LocalRotation(const glm::quat &rotation);
	glm::vec3 LocalScale() const;
	void LocalScale(const glm::vec3 &scale);

	// Transformations
	glm::mat4 ModelToWorld() const;
	glm::mat4 WorldToModel() const;

	// Directions
	glm::vec3 Up() const;
	glm::vec3 Forward() const;
	glm::vec3 Right() const;

	// Parenting
	TransformComponent* GetParent() const;
	void SetParent(TransformComponent* parent);

	/// Rotation of <i>angle</i> degrees around <i>axis</i> around the local origin
	/// \param angle How many degrees to rotate
	/// \param axis Vector around which to rotate
	void AngleAxis(float angle, const glm::vec3& axis);

	/// Rotation of <i>angle</i> degrees around <i>axis</i> around origin <i>point</i>
	/// \param angle How many degrees to rotate
	/// \param axis Vector around which to rotate
	/// \param point Rotation origin
	void AngleAxisPoint(float angle, const glm::vec3& axis, const glm::vec3& point);

	/// Rotate to face <i>direction</i>, aligning with <i>up</i>
	/// @param direction Direction to face
	/// @param up Vector to align own up vector with
	void LookAt(glm::vec3 direction, glm::vec3 up);

	/// Align position and rotation to another transform
	/// @param other Transform to align with
	void AlignWith(const TransformComponent& other);

private:
	friend class Serializer;

	glm::vec3 _position = glm::vec3(0);
	glm::quat _rotation = glm::quat(glm::vec3(0));
	glm::vec3 _scale = glm::vec3(1);

	TransformComponent* _parent = nullptr;

	mutable glm::mat4 _modelToWorld = glm::mat4(0);
	mutable glm::mat4 _worldToModel = glm::mat4(0);
	mutable bool _valid = false;

	void Update() const;
};


#endif