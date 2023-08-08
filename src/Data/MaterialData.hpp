#ifndef OGL_ENGINE_MATERIALDATA_HPP
#define OGL_ENGINE_MATERIALDATA_HPP

#include <map>
#include "ShaderData.hpp"
#include <glm/glm.hpp>


class MaterialData
{
public:
	explicit MaterialData(const ShaderData &shader);

	void Bind();

	float &FloatProperty(const std::string &name);

	glm::vec3 &Vec3Property(const std::string &name);

	glm::vec4 &Vec4Property(const std::string &name);

	glm::mat4 &Mat4Property(const std::string &name);

private:
	ShaderData _shader;
	std::map<std::string, float> _propertiesFloat;
	std::map<std::string, glm::vec3> _propertiesVec3;
	std::map<std::string, glm::vec4> _propertiesVec4;
	std::map<std::string, glm::mat4> _propertiesMat4;
};


#endif