#ifndef OGL_ENGINE_TRANSFORM_HPP
#define OGL_ENGINE_TRANSFORM_HPP
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Transform
{
public:
	const glm::vec3 &Position() const;
	glm::vec3 &Position();

	const glm::quat &Rotation() const;
	glm::quat &Rotation();

	const glm::vec3 &Scale() const;
	glm::vec3 &Scale();

private:
	glm::vec3 _position;
	glm::quat _rotation;
	glm::vec3 _scale;

	bool _changed;
};


#endif //OGL_ENGINE_TRANSFORM_HPP
