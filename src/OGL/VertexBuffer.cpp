#include "VertexBuffer.hpp"
#include "GL/glew.h"


const VertexBuffer VertexBuffer::None = VertexBuffer();

VertexBuffer::VertexBuffer() :_vbo(0)
{

}

VertexBuffer::VertexBuffer(const void *data, unsigned int size): _vbo(0)
{
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_vbo);
}

GLuint VertexBuffer::GetBindID() const
{
    return _vbo;
}
