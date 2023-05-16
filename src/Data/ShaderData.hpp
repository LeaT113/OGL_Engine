#ifndef OGL_ENGINE_SHADERDATA_HPP
#define OGL_ENGINE_SHADERDATA_HPP

#include <string>
#include <GL/glew.h>


class ShaderData
{
public:
	ShaderData(const std::string &vertexShader, const std::string &fragmentShader);

private:
	static GLuint CompileShader(GLenum type, const std::string &source);
};


#endif
