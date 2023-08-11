#include "VertexBuffer.hpp"
#include "GL/glew.h"
#include "Graphics.hpp"


const VertexBuffer VertexBuffer::None = VertexBuffer();

VertexBuffer::VertexBuffer() :_vbo(0), _size(0)
{

}

VertexBuffer::VertexBuffer(const void *data, unsigned int size): _vbo(0), _size(size)
{
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_vbo);
}


void VertexBuffer::InsertData(unsigned int offset, const void *data, unsigned int size)
{
    if(offset+size > _size)
        throw std::runtime_error("VertexBuffer::InsertData out of reserved memory range");

    Graphics::Bind(*this);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::InsertAttribute(VertexAttribute attribute, unsigned int &offset,
                                   const void *data, unsigned int vertexCount)
{
    unsigned int byteSize = vertexCount * GetVertexAttributeInfo(attribute).byteSize;
    InsertData(offset, data, byteSize);
    offset += byteSize;
}

GLuint VertexBuffer::GetBindID() const
{
    return _vbo;
}
