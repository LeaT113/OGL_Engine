#include "Texture.hpp"

#include "../OGL/Graphics.hpp"

unsigned int GetTexTarget(Texture::Type type)
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

std::tuple<int, unsigned int, unsigned int> GetFormatInfo(Texture::Format format, bool sRGB)
{
    switch (format)
    {
    case Texture::Format::R8:
        return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };

    case Texture::Format::RGB8:
        return { sRGB ? GL_SRGB8 : GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE };

    case Texture::Format::RGBA8:
        return { sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };

    default:
        return { 0, 0, 0 };
    }
}

Texture::Params Texture::DefaultParams = { true, true, Tiling::Repeat, Filtering::Trilinear };

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

Texture::Texture(Type type, Format format, unsigned int width, unsigned int height, std::string name, void* data, Params params)
    : _type(type), _format(format), _width(width), _height(height), Resource(std::move(name)), _params(params)
{
    glGenTextures(1, &_textureId);
    auto target = static_cast<unsigned int>(_type);
    glBindTexture(target, _textureId);

    auto [minFilter, magFilter] = GetMinMagFilter(_params.filtering, _params.mipmaps);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);

    auto tiling = static_cast<int>(_params.tiling);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, tiling);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, tiling);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, tiling);

    if (_type != Type::TexCubemap)
        Fill(data);
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

    unsigned int target = GetTexTarget(_type);
    if (_type == Type::TexCubemap)
        target += index;

    auto [internalFormat, format, dataType] = GetFormatInfo(_format, _params.sRGB);

    if (_invalid)
        glTexImage2D(target, 0, internalFormat, _width, _height, 0, format, dataType, data);
    else
        glTexSubImage2D(target, 0, 0, 0, _width, _height, format, dataType, data);

    if (_params.mipmaps)
        if (_type != Type::TexCubemap || index == 5)
            glGenerateMipmap(static_cast<unsigned int>(_type));

    if (_type != Type::TexCubemap)
        _invalid = false;
}

unsigned Texture::GetBindID() const
{
    return _textureId;
}

Texture::Type Texture::GetType() const
{
    return _type;
}

