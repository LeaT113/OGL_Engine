#include <iostream>
#include <map>
#include "ShaderData.hpp"
#include <glm/gtc/type_ptr.hpp>


ShaderData::ShaderData(const std::set<std::string> &uniforms, const std::string &vertexShader,
					   const std::string &fragmentShader)
{
	_shaderProgram = glCreateProgram();
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(_shaderProgram, vertex);
	glAttachShader(_shaderProgram, fragment);
	glLinkProgram(_shaderProgram);
	glValidateProgram(_shaderProgram);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Uniforms
	// TODO properly https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#ideal-way-of-retrieving-all-uniform-names
	for (const std::string &uniform: uniforms)
	{
		auto findSplit = uniform.find(' ');

		auto type = UniformTypeFromGlsl(uniform.substr(0, findSplit));
		auto name = uniform.substr(findSplit + 1, uniform.length() - findSplit - 1);
		auto location = glGetUniformLocation(_shaderProgram, name.c_str());

		_uniforms.emplace(name, std::make_pair(type, location));
	}
}

ShaderData::~ShaderData()
{
	glDeleteProgram(_shaderProgram);
}


void ShaderData::Bind() const
{
	glUseProgram(_shaderProgram);
}

void ShaderData::SetFloat(const std::string &name, float value)
{
	auto it = _uniforms.find(name);
	if(it == _uniforms.end() || it->second.first != UniformType::Float || it->second.second == -1)
	{
		std::cout << "ShaderData non-existent float uniform: " << name;
		return;
	}

	glUniform1f(it->second.second, value);
}

void ShaderData::SetVec3(const std::string &name, glm::vec3 value)
{
	auto it = _uniforms.find(name);
	if(it == _uniforms.end() || it->second.first != UniformType::Vec3 || it->second.second == -1)
	{
		std::cout << "ShaderData non-existent vec3 uniform: " << name;
		return;
	};

	glUniform3fv(it->second.second, 1, glm::value_ptr(value));
}

void ShaderData::SetVec4(const std::string &name, glm::vec4 value)
{
	auto it = _uniforms.find(name);
	if(it == _uniforms.end() || it->second.first != UniformType::Vec4 || it->second.second == -1)
	{
		std::cout << "ShaderData non-existent vec4 uniform: " << name;
		return;
	};

	glUniform4fv(it->second.second, 1, glm::value_ptr(value));
}

void ShaderData::SetMat4(const std::string &name, glm::mat4 value)
{
	auto it = _uniforms.find(name);
	if(it == _uniforms.end() || it->second.first != UniformType::Mat4 || it->second.second == -1)
	{
		std::cout << "ShaderData non-existent mat4 uniform: " << name;
		return;
	};

	glUniformMatrix4fv(it->second.second, 1, GL_FALSE, glm::value_ptr(value));
}


GLuint ShaderData::CompileShader(GLenum type, const std::string &source)
{
	GLuint shader = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *) alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		std::string mes = "ShaderData failed to compile: ";
		mes += message;
		throw std::runtime_error(mes);
	}

	return shader;
}

ShaderData::UniformType ShaderData::UniformTypeFromGlsl(const std::string &typeName)
{
	const static std::map<std::string, UniformType> uniformTypeForString = {
			{"float", UniformType::Float},
			{"vec3",  UniformType::Vec3},
			{"vec4",  UniformType::Vec4},
			{"mat4",  UniformType::Mat4}
	};

	auto it = uniformTypeForString.find(typeName);
	if (it == uniformTypeForString.end())
		return UniformType::Invalid;

	return it->second;
}

const std::unordered_map<std::string, std::pair<ShaderData::UniformType, GLint>> &ShaderData::Uniforms() const
{
	return _uniforms;
}
