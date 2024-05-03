#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

#include "Resource.hpp"
#include "../OGL/IBindable.hpp"


class FrameBuffer;

class Texture : public Resource, public IBindable
{
public:
    enum class Type
    {
        Tex1D = GL_TEXTURE_1D,
        Tex2D = GL_TEXTURE_2D,
        Tex3D = GL_TEXTURE_3D,
        TexCubemap = GL_TEXTURE_CUBE_MAP,
    };

    enum class Tiling
    {
        Clamp = GL_CLAMP_TO_BORDER,
        Extend = GL_CLAMP_TO_EDGE,
        Repeat = GL_REPEAT,
        MirrorRepeat = GL_MIRRORED_REPEAT
    };

    enum class Filtering
    {
        Point,
        Bilinear,
        Trilinear
    };

    enum class Format
    {
        None = 0,

        R8,
        R32UI,
        RGB8,
        RGBA8,

        RGBA16F,

        // Depth
        Depth16,
        Depth24Stencil8,
        Depth32,
        Depth32F,
    };

    struct Params
    {
        bool mipmaps = true;
        bool sRGB = true;
        Tiling tiling = Tiling::Repeat;
        Filtering filtering = Filtering::Trilinear;
    };

    static Params DefaultParams;

    Texture(Type type, Format format, unsigned int width, unsigned int height, std::string name, void* data = nullptr, Params params = DefaultParams);
    ~Texture() override;

    void Resize(unsigned int width, unsigned int height);
    void Fill(void* data, int index = 0);
    void Copy(const FrameBuffer &buffer, int subIndex = 0);

    unsigned GetBindID() const override;

    Type GetType() const;

    static bool IsDepthFormat(Format format);
private:
    struct FormatInfo
    {
        int internalFormat;
        unsigned int format;
        unsigned int dataType;
    };
    static FormatInfo GetFormatInfo(Format format, bool sRGB = false);

    GLuint _textureId = 0;

    unsigned int _width = 0, _height = 0;

    Type _type;
    Format _format;
    Params _params;

    bool _invalid = true;
};

#endif
