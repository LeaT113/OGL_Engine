#ifndef OGL_ENGINE_VERTEXARRAY_HPP
#define OGL_ENGINE_VERTEXARRAY_HPP

#include "IBindable.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexAttributes.hpp"


class VertexArray : public IBindable
{
public:
    enum Layout
    {
        Packed,
        Interleaved
    };

    explicit VertexArray(Layout layout, size_t vertexCount);
    ~VertexArray() override;

    GLuint GetBindID() const override;

    void AddVertexBuffer(const VertexBuffer &buffer, const std::vector<VertexAttribute> &attributeLayout);
    void SetIndexBuffer(const IndexBuffer &buffer);

    size_t GetVertexCount() const;

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
