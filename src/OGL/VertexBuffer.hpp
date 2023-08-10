#ifndef OGL_ENGINE_VERTEXBUFFER_HPP
#define OGL_ENGINE_VERTEXBUFFER_HPP

#include "GL/glew.h"
#include "IBindable.hpp"

// TODO Add subdata support
class VertexBuffer : public IBindable
{
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    GLuint GetBindID() const override;

    const static VertexBuffer None;

private:
    VertexBuffer();

    GLuint _vbo;
};


#endif
