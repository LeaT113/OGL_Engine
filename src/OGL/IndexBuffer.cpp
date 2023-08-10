#include "IndexBuffer.hpp"


const IndexBuffer IndexBuffer::None = IndexBuffer();

IndexBuffer::IndexBuffer() :_ebo(0)
{

}

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count): _ebo(0)
{
    glGenBuffers(1, &_ebo);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &_ebo);
}

GLuint IndexBuffer::GetBindID() const
{
    return _ebo;
}



