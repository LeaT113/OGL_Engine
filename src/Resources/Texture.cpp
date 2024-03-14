#include "Texture.hpp"

Texture::Texture(unsigned int id, std::string name) : _textureId(id)
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
