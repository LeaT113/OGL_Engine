#include "Texture.hpp"

#include "../OGL/FrameBuffer.hpp"
#include "../OGL/Graphics.hpp"

unsigned int GetTextureDataTarget(Texture::Type type)
{
    switch (type)
    {
    case Texture::Type::Tex1D:
    case Texture::Type::Tex2D:
    case Texture::Type::Tex3D:
        return static_cast<unsigned int>(type);

    case Texture::Type::TexCubemap:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_X;

    default:
        return 0;
    }
}

std::pair<int, int> GetMinMagFilter(Texture::Filtering filtering, bool mipmaps)
{
    switch (filtering)
    {
    case Texture::Filtering::Point:
        return { mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST, GL_NEAREST };

    case Texture::Filtering::Bilinear:
        return { mipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR, GL_LINEAR };

    case Texture::Filtering::Trilinear:
        return { mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR, GL_LINEAR };

        default:
            return { 0, 0 };
    }
}

Texture::Texture(Type type, Format format, Params params)
    : _type(type), _format(format), _width(params.width), _height(params.height), Resource(std::move(params.name)), _settings(params.settings)
{
    glGenTextures(1, &_textureId);
    if (_textureId == 0)
        throw std::runtime_error("Texture creation failed");
    auto target = static_cast<unsigned int>(_type);
    glBindTexture(target, _textureId);

    auto [minFilter, magFilter] = GetMinMagFilter(_settings.filtering, _settings.mipmaps);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);

    auto tiling = static_cast<int>(_settings.tiling);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, tiling);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, tiling);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, tiling);

    // TODO Should this be called when dimensions are 0?
    if (_type != Type::TexCubemap)
        Fill(params.data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureId);
}

void Texture::Resize(unsigned int width, unsigned int height)
{
    _width = width;
    _height = height;
    _invalid = true;
}

void Texture::Fill(void* data, int index)
{
    glBindTexture(static_cast<unsigned int>(_type), _textureId);

    unsigned int target = GetTextureDataTarget(_type);
    if (_type == Type::TexCubemap)
        target += index;

    auto formatInfo = GetFormatInfo(_format, _settings.sRGB);

    if (_invalid)
        glTexImage2D(target, 0, formatInfo.internalFormat, static_cast<int>(_width), static_cast<int>(_height), 0, formatInfo.format, formatInfo.dataType, data);
    else
        glTexSubImage2D(target, 0, 0, 0, static_cast<int>(_width), static_cast<int>(_height), formatInfo.format, formatInfo.dataType, data);

    if (_type != Type::TexCubemap || index == 5)
    {
        if (_settings.mipmaps && data != nullptr)
            glGenerateMipmap(static_cast<unsigned int>(_type));

        _invalid = false;
    }
}

void Texture::Copy(const FrameBuffer& buffer, int subIndex)
{
    glBindTexture(static_cast<unsigned int>(_type), _textureId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.GetBindID());
    // if (type != CopyType::Default)
    //     glReadBuffer(static_cast<unsigned int>(type));

    unsigned int target = GetTextureDataTarget(_type);
    if (_type == Type::TexCubemap)
        target += subIndex;

    auto formatInfo = GetFormatInfo(_format, _settings.sRGB);

    if (_invalid)
        glCopyTexImage2D(target, 0, formatInfo.internalFormat, 0, 0, static_cast<int>(_width), static_cast<int>(_height), 0);
    else
        glCopyTexSubImage2D(target, 0, 0, 0, 0 , 0, static_cast<int>(_width), static_cast<int>(_height));

    if (_type != Type::TexCubemap || subIndex == 5)
    {
        if (_settings.mipmaps)
            glGenerateMipmap(static_cast<unsigned int>(_type));

        _invalid = false;
    }
}

unsigned Texture::GetBindID() const
{
    return _textureId;
}

Texture::Type Texture::GetType() const
{
    return _type;
}

bool Texture::IsDepthFormat(Format format)
{
    auto internalFormat = GetFormatInfo(format, false).format;
    return internalFormat == GL_DEPTH_COMPONENT || internalFormat == GL_DEPTH_STENCIL;
}

Texture::FormatInfo Texture::GetFormatInfo(Format format, bool sRGB)
{
    switch (format)
    {
    case Format::R8:
        return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };

    case Format::R32UI:
        return { GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT };

    case Format::RGB8:
        return { sRGB ? GL_SRGB8 : GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE };

    case Format::RGBA8:
        return { sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };

    case Format::RGBA16F:
        return { GL_RGBA16F, GL_RGBA, GL_FLOAT };

    // Depth
    case Format::Depth16:
        return { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT };

    case Format::Depth24Stencil8:
        return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };

    case Format::Depth32:
        return { GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT };

    case Format::Depth32F:
        return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };

    default:
        throw std::runtime_error("Texture Unknown format");
    }
}

