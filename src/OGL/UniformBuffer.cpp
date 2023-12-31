#include "UniformBuffer.hpp"


const UniformBuffer UniformBuffer::None = UniformBuffer();

UniformBuffer::UniformBuffer() : _ubo(0), _size(0)
{

}

UniformBuffer::UniformBuffer(unsigned int size) : _ubo(0), _size(size)
{
    glGenBuffers(1, &_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &_ubo);
}

void UniformBuffer::SetData(unsigned offset, unsigned size, const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

GLuint UniformBuffer::GetBindID() const
{
    return _ubo;
}
