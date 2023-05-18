#ifndef OGL_ENGINE_SHADERDATA_HPP
#define OGL_ENGINE_SHADERDATA_HPP

#include <string>
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>
#include <unordered_map>
#include <set>


class ShaderData
{
public:
	enum class UniformType
	{
		Invalid = 0,
		Float = 1,
		Vec3 = 2,
		Vec4 = 3,
		Mat4 = 4
	};

	ShaderData(const std::set<std::string> &uniforms, const std::string &vertexShader, const std::string &fragmentShader);

	~ShaderData();

	void Bind() const;

	void SetFloat(const std::string &name, float value);

	void SetVec3(const std::string &name, glm::vec3 value);

	void SetVec4(const std::string &name, glm::vec4 value);

	void SetMat4(const std::string &name, glm::mat4 value);

	const std::unordered_map<std::string, std::pair<ShaderData::UniformType, GLint>> &Uniforms() const;

private:
	// TODO Track bound shader as static member
	std::unordered_map<std::string, std::pair<UniformType, GLint>> _uniforms;
	GLuint _shaderProgram;

	static GLuint CompileShader(GLenum type, const std::string &source);

	static UniformType UniformTypeFromGlsl(const std::string &typeName);
};


#endif
