#ifndef OGL_ENGINE_MESHDATA_HPP
#define OGL_ENGINE_MESHDATA_HPP

#include <GL/glew.h>

class MeshData
{
public:
	MeshData(uint32_t vertexCount, const float *positions, const float *normals,
             uint32_t triangleCount, const unsigned int *indices);

	~MeshData();

	void Bind() const;

	void Draw() const;

private:
	static constexpr GLuint PositionLocation = 0;
	static constexpr GLuint NormalLocation = 1;

	uint32_t _triangleCount;
	GLuint _vbo;
	GLuint _ebo;
	GLuint _vao;
};


#endif
