#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

#include "Resource.hpp"
#include "../OGL/IBindable.hpp"



class Texture : public Resource, public IBindable
{
public:
    enum Type
    {
        Tex1D,
        Tex2D,
        Tex3D,
        TexCubemap,
        Tex1DArray,
        Tex2DArray,
    };

    explicit Texture(unsigned int id, std::string name);
    ~Texture() override;

    unsigned GetBindID() const override;

private:
    GLuint _textureId;
};

#endif
