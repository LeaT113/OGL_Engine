#ifndef OGL_ENGINE_INDEXBUFFER_HPP
#define OGL_ENGINE_INDEXBUFFER_HPP

#include <vector>

#include "GL/glew.h"
#include "IBindable.hpp"


class IndexBuffer : public IBindable
{
public:
    explicit IndexBuffer(const std::vector<unsigned int> &indices);
    ~IndexBuffer() override;

    GLuint GetBindID() const override;

    const static IndexBuffer None;

private:
    IndexBuffer();

    GLuint _ebo = 0;
};


#endif
