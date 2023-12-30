#ifndef UNIFORMBUFFER_HPP
#define UNIFORMBUFFER_HPP

#include <GL/glew.h>
#include "IBindable.hpp"


class UniformBuffer : public IBindable
{
public:
    explicit UniformBuffer(unsigned int size);
    ~UniformBuffer();

    void SetData(unsigned int offset, unsigned int size, const void *data);

    GLuint GetBindID() const override;

    const static UniformBuffer None;
private:
    UniformBuffer();

    GLuint _ubo;
    unsigned int _size;
};



#endif //UNIFORMBUFFER_HPP
