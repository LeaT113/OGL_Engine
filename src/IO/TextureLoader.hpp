#ifndef TEXTURELOADER_HPP
#define TEXTURELOADER_HPP

#include "../Core/Handle.hpp"
#include "../Resources/Texture.hpp"

class Texture;

class TextureLoader
{
public:
    static Handle<Texture> LoadTexture2D(const std::string &name, const Texture::Settings& settings = Texture::Settings::Default());
    static Handle<Texture> LoadCubemap(const std::string& name, const Texture::Settings& settings);

private:
    static const std::string TexturesPath;
};



#endif
