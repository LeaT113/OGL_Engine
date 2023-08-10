#include <cassert>
#include <stdexcept>
#include "VertexArray.hpp"
#include "Graphics.hpp"


const VertexArray VertexArray::None = VertexArray();

VertexArray::VertexArray(VertexArray::Layout layout, size_t vertexCount) : _layout(layout), _vertexCount(vertexCount), _vao(0)
{
    glGenVertexArrays(1, &_vao);
}

VertexArray::VertexArray(): _layout(Layout::Interleaved), _vertexCount(0), _vao(0)
{

}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &_vao);
}


GLuint VertexArray::GetBindID() const
{
    return _vao;
}


void VertexArray::AddVertexBuffer(const VertexBuffer &buffer, const std::vector<Shader::Attribute> &attributeLayout)
{
    if(_vertexBufferCount > 0)
        throw std::runtime_error("VertexArray currently only supports one VertexBuffer");

    Graphics::Bind(*this);

    Graphics::Bind(buffer);

    int stride = 0;
    if(_layout == Layout::Interleaved)
    {
        for(auto attribute : attributeLayout)
            stride += Shader::GetAttributeInfo(attribute).byteSize;
    }

    size_t offset = 0;

    for (auto attribute : attributeLayout)
    {
        auto attributeInfo = Shader::GetAttributeInfo(attribute);

        glVertexAttribPointer((GLuint) attribute, attributeInfo.size, attributeInfo.type, attributeInfo.normalized, stride, (void *) offset);
        glEnableVertexAttribArray((GLuint) attribute);

        offset += _layout == Layout::Interleaved ? attributeInfo.byteSize : attributeInfo.byteSize * _vertexCount;
    }

    Graphics::Bind(VertexArray::None);

    _vertexBufferCount++;
}

void VertexArray::SetIndexBuffer(const IndexBuffer &buffer)
{
    Graphics::Bind(*this);

    Graphics::Bind(buffer);

    Graphics::Bind(VertexArray::None);

    _indexBufferPresent = true;
}

