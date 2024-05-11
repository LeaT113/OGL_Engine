#ifndef OGL_ENGINE_VERTEXBUFFER_HPP
#define OGL_ENGINE_VERTEXBUFFER_HPP

#include "GL/glew.h"
#include "IBindable.hpp"
#include "VertexAttributes.hpp"


class VertexBuffer : public IBindable
{
public:
    explicit VertexBuffer(unsigned int size);
    ~VertexBuffer() override;

    void InsertAttribute(VertexAttribute attribute, const void* data, unsigned int count);

    GLuint GetBindID() const override;

    const static VertexBuffer None;

private:
    VertexBuffer();

    GLuint _vbo = 0;
    unsigned int _size = 0;
    unsigned int _insertOffset = 0;
};


#endif
