#include <stdexcept>
#include "MaterialData.hpp"


MaterialData::MaterialData(ShaderData &shader)
{
	for (const auto &uniform : shader.Uniforms())
	{
		switch(uniform.second.first)
		{
			case ShaderData::UniformType::Invalid:
				throw std::runtime_error("MaterialData invalid uniform type from shader");

			case ShaderData::UniformType::Float:
				_propertiesFloat.emplace(uniform.first, 0);
				break;

			case ShaderData::UniformType::Vec3:
				_propertiesVec3.emplace(uniform.first, glm::vec3());
				break;

			case ShaderData::UniformType::Vec4:
				_propertiesVec4.emplace(uniform.first, glm::vec4());
				break;

			case ShaderData::UniformType::Mat4:
				_propertiesMat4.emplace(uniform.first, glm::mat4());
				break;
		}
	}
}


float &MaterialData::FloatProperty(const std::string &name)
{
	auto it = _propertiesFloat.find(name);
	if(it == _propertiesFloat.end())
		throw std::runtime_error("MaterialData invalid float property: " + name);

	return it->second;
}

glm::vec3 &MaterialData::Vec3Property(const std::string &name)
{
	auto it = _propertiesVec3.find(name);
	if(it == _propertiesVec3.end())
		throw std::runtime_error("MaterialData invalid vec3 property: " + name);

	return it->second;
}

glm::vec4 &MaterialData::Vec4Property(const std::string &name)
{
	auto it = _propertiesVec4.find(name);
	if(it == _propertiesVec4.end())
		throw std::runtime_error("MaterialData invalid vec4 property: " + name);

	return it->second;
}

glm::mat4 &MaterialData::Mat4Property(const std::string &name)
{
	auto it = _propertiesMat4.find(name);
	if(it == _propertiesMat4.end())
		throw std::runtime_error("MaterialData invalid mat4 property: " + name);

	return it->second;
}
