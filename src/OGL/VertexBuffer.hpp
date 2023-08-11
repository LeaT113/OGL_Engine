#ifndef OGL_ENGINE_VERTEXBUFFER_HPP
#define OGL_ENGINE_VERTEXBUFFER_HPP

#include "GL/glew.h"
#include "IBindable.hpp"
#include "VertexAttributes.hpp"


class VertexBuffer : public IBindable
{
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void InsertData(unsigned int offset, const void* data, unsigned int size);

    void InsertAttribute(VertexAttribute attribute, unsigned int &offset, const void* data, unsigned int vertexCount);

    GLuint GetBindID() const override;

    const static VertexBuffer None;

private:
    VertexBuffer();

    GLuint _vbo;
    unsigned int _size;
};


#endif
