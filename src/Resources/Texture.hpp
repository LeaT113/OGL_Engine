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

    enum class CopyType
    {
        Depth = GL_NONE,

        Color0 = GL_COLOR_ATTACHMENT0,
        Color1 = GL_COLOR_ATTACHMENT1,
        Color2 = GL_COLOR_ATTACHMENT2,
        Color3 = GL_COLOR_ATTACHMENT3,

        Default,
    };

    struct Settings
    {
        bool mipmaps = true;
        bool sRGB = true;
        Tiling tiling = Tiling::Repeat;
        Filtering filtering = Filtering::Trilinear;

        static Settings Default() { return {}; }
    };

    struct Params
    {
        unsigned int width = 0;
        unsigned int height = 0;
        std::string name;
        void* data = nullptr;
        Settings settings = Settings::Default();

        static Params Default() { return {}; }
    };

    Texture(Type type, Format format, Params params = Params::Default());
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
    Settings _settings;

    bool _invalid = true;
};

#endif
