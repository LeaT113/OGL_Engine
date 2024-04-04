#include "Texture.hpp"

Texture::Texture(unsigned int id, std::string name, Type type) : _textureId(id), _type(type)
{
    _name = std::move(name);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureId);
}

unsigned Texture::GetBindID() const
{
    return _textureId;
}

Texture::Type Texture::GetType() const
{
    return _type;
}
