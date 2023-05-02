#ifndef OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#define OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#include "Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class TransformComponent : public Component
{
public:
	explicit TransformComponent(const Entity* owner);

	const glm::vec3 &Position() const;
	glm::vec3 &Position();

	const glm::quat &Rotation() const;
	glm::quat &Rotation();

	const glm::vec3 &Scale() const;
	glm::vec3 &Scale();

	const glm::mat4 &ModelToWorld();

	void NotifyParentChanged();

	/// Rotation of <i>angle</i> degrees around <i>axis</i> around the local origin
	/// \param angle How many degrees to rotate
	/// \param axis Vector around which to rotate
	void AngleAxis(float angle, const glm::vec3& axis);

	/// Rotation of <i>angle</i> degrees around <i>axis</i> around <i>point</i>
	/// \param angle How many degrees to rotate
	/// \param axis Vector around which to rotate
	/// \param point Rotation origin
	void AngleAxisPoint(float angle, const glm::vec3& axis, const glm::vec3& point);

private:
	glm::vec3 _position;
	glm::quat _rotation;
	glm::vec3 _scale;
	glm::mat4 _modelToWorld;

	bool _changed = true;
};


#endif //OGL_ENGINE_TRANSFORMCOMPONENT_HPP
