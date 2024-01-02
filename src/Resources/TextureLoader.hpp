#ifndef TEXTURELOADER_HPP
#define TEXTURELOADER_HPP

#include "../Core/Handle.hpp"

class Texture;

class TextureLoader
{
public:
    static Handle<Texture> LoadTexture(const std::string &path);

private:
    static const std::string TexturesPath;
};



#endif //TEXTURELOADER_HPP
