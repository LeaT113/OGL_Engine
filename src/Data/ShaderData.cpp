#include <iostream>
#include "ShaderData.hpp"


ShaderData::ShaderData(const std::string &vertexShader, const std::string &fragmentShader)
{
	GLuint program = glCreateProgram();
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

GLuint ShaderData::CompileShader(GLenum type, const std::string &source)
{
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		std::string mes = "ShaderData failed to compile: ";
		mes += message;
		throw std::runtime_error(mes);
	}

	return shader;
}
