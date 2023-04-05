#ifndef OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#define OGL_ENGINE_TRANSFORMCOMPONENT_HPP
#include "Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class TransformComponent : public Component
{
public:
	TransformComponent();

	const glm::vec3 &Position() const;
	glm::vec3 &Position();

	const glm::quat &Rotation() const;
	glm::quat &Rotation();

	const glm::vec3 &Scale() const;
	glm::vec3 &Scale();

private:
	glm::vec3 _position{};
	glm::quat _rotation{};
	glm::vec3 _scale{};

	bool _changed = true;
};


#endif //OGL_ENGINE_TRANSFORMCOMPONENT_HPP
