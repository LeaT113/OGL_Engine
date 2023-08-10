#ifndef OGL_ENGINE_VERTEXARRAY_HPP
#define OGL_ENGINE_VERTEXARRAY_HPP

#include <vector>
#include "IBindable.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "../Resources/Shader.hpp"


class VertexArray : public IBindable
{
public:
    enum Layout
    {
        Packed,
        Interleaved
    };

    explicit VertexArray(Layout layout, size_t vertexCount);
    ~VertexArray();

    GLuint GetBindID() const override;

    void AddVertexBuffer(const VertexBuffer &buffer, const std::vector<Shader::Attribute> &attributeLayout);
    void SetIndexBuffer(const IndexBuffer &buffer);

    const static VertexArray None;

private:
    VertexArray();

    Layout _layout;
    size_t _vertexCount;
    GLuint _vao;
    unsigned int _vertexBufferCount = 0;
    bool _indexBufferPresent = false;
};


#endif
