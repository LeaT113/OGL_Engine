#ifndef OGL_ENGINE_INDEXBUFFER_HPP
#define OGL_ENGINE_INDEXBUFFER_HPP

#include "GL/glew.h"
#include "IBindable.hpp"


class IndexBuffer : public IBindable
{
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    GLuint GetBindID() const override;

    const static IndexBuffer None;

private:
    IndexBuffer();

    GLuint _ebo;
};


#endif
