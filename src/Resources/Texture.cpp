#include "Texture.hpp"

Texture::Texture(unsigned int id) : _textureId(id)
{}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureId);
}

unsigned Texture::GetBindID() const
{
    return _textureId;
}
