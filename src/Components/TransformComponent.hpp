#ifndef OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#define OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#include "Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class TransformComponent : public Component
{
public:
	explicit TransformComponent(const Entity& owner);

	glm::vec3 Position() const;
	void Position(const glm::vec3 &position);
	glm::quat Rotation() const;
	void Rotation(const glm::quat &rotation);
	glm::vec3 Scale() const;
	void Scale(const glm::vec3 &scale);

	glm::vec3 LocalPosition() const;
	void LocalPosition(const glm::vec3 &position);
	glm::quat LocalRotation() const;
	void LocalRotation(const glm::quat &rotation);

	void SetParent(TransformComponent* parent);
	TransformComponent* GetParent() const;

	glm::vec3 Up() const;
	glm::vec3 Forward() const;
	glm::vec3 Right() const;

	glm::mat4 ModelToWorld() const;
	glm::mat4 WorldToModel() const;

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