#include "IndexBuffer.hpp"


const IndexBuffer IndexBuffer::None = IndexBuffer();

IndexBuffer::IndexBuffer() :_ebo(0)
{

}

IndexBuffer::IndexBuffer(const std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &_ebo);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &_ebo);
}

GLuint IndexBuffer::GetBindID() const
{
    return _ebo;
}



