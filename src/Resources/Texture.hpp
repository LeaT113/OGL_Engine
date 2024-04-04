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

    enum Repeat
    {
        Clamp,
        Extend,
        Repeat,
        MirrorRepeat
    };

    struct Params
    {
        bool Mipmaps = true;
        bool sRGB = true;
        enum Repeat Repeat = Repeat::Repeat;
        // TODO BitDepth like byte, float? for hdr, Point sampling
    };

    explicit Texture(unsigned int id, std::string name, Type type);
    ~Texture() override;

    unsigned GetBindID() const override;

    Type GetType() const;

private:
    GLuint _textureId;
    Type _type;
};

#endif
