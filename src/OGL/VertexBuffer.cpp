#include "VertexBuffer.hpp"
#include "GL/glew.h"
#include "Graphics.hpp"


const VertexBuffer VertexBuffer::None = VertexBuffer();

VertexBuffer::VertexBuffer() = default;

VertexBuffer::VertexBuffer(unsigned int size): _size(size)
{
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_vbo);
}

/// Insert an attribute with data
/// @param attribute Attribure to insert
/// @param data Pointer to raw data
/// @param count Vertex count if vertex attribute or matrix count if instance transforms
void VertexBuffer::InsertAttribute(VertexAttribute attribute, const void *data, unsigned int count)
{
    unsigned int size = count * GetVertexAttributeInfo(attribute).byteSize;
    if(_insertOffset + size > _size)
        throw std::runtime_error("VertexBuffer::InsertAttribute out of reserved memory range");

    Graphics::Bind(*this);
    glBufferSubData(GL_ARRAY_BUFFER, _insertOffset, size, data);
    _insertOffset += size;
}

GLuint VertexBuffer::GetBindID() const
{
    return _vbo;
}
