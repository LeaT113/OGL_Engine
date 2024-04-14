#ifndef TEXTURELOADER_HPP
#define TEXTURELOADER_HPP

#include <vector>

#include "../Core/Handle.hpp"
#include "../Resources/Texture.hpp"

class Texture;

class TextureLoader
{
public:
    static Handle<Texture> LoadTexture2D(const std::string &name, const Texture::Params& params = Texture::DefaultParams);
    static Handle<Texture> LoadCubemap(const std::string& name, const Texture::Params& params);

private:
    static const std::string TexturesPath;

    static void LoadImageToTex2D(const std::string& absolutePath, const Texture::Params& params, unsigned int target);
};



#endif //TEXTURELOADER_HPP
