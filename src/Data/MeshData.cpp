#include "MeshData.hpp"

MeshData::MeshData(uint32_t vertexCount, const float *vertices, const float *normals,
				   uint32_t triangleCount, const unsigned int *indices) : _triangleCount(triangleCount),
																		  _vbo(0), _ebo(0), _vao(0)
{
	const GLsizeiptr vertexPositionsSize = 3 * sizeof(float) * vertexCount;
	const GLsizeiptr vertexNormalsSize = 3 * sizeof(float) * vertexCount;
	const GLsizeiptr indicesSize = 3 * sizeof(unsigned int) * triangleCount;

	// VBO
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexPositionsSize + vertexNormalsSize, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexPositionsSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertexPositionsSize, vertexNormalsSize, normals);

	// EBO
	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

	// VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	{
		// Buffers
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

		// Attributes
		glVertexAttribPointer(PositionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *) vertexPositionsSize);
		glEnableVertexAttribArray(1);
	}
	glBindVertexArray(0);
}

MeshData::~MeshData()
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
}
