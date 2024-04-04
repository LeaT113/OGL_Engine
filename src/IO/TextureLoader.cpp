#include "TextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <format>
#include <stb_image.h>
#include <GL/glew.h>
#include "../Resources/Texture.hpp"

const std::string TextureLoader::TexturesPath = "res/Textures/";

Handle<Texture> TextureLoader::LoadTexture2D(const std::string &name, const Texture::Params& params)
{
    stbi_set_flip_vertically_on_load(1);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    LoadImageToTex2D(std::format("{}{}.png", TexturesPath, name), params, GL_TEXTURE_2D);

    SetTextureParams(params, GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Handle<Texture>::Make(texture, name, Texture::Type::Tex2D);
}

Handle<Texture> TextureLoader::LoadCubemap(const std::string& name, const Texture::Params& params)
{
    stbi_set_flip_vertically_on_load(0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    static std::string suffixes[6] = {"px", "nx", "py", "ny", "pz", "nz"};
    for (int i = 0; i < 6; i++)
    {
        LoadImageToTex2D(std::format("{}{}_{}.png", TexturesPath, name, suffixes[i]), params, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
    }
    SetTextureParams(params, GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return Handle<Texture>::Make(texture, name, Texture::Type::TexCubemap);
}

void TextureLoader::LoadImageToTex2D(const std::string& absolutePath, const Texture::Params& params, unsigned int target)
{
    int width, height, channelCount;
    const auto imageData = stbi_load(absolutePath.c_str(), &width, &height, &channelCount, 0);
    if (!imageData)
        throw std::runtime_error("TextureLoader Failed to load image");

    GLint internalFormat = 0;
    GLenum format = 0;
    if(channelCount == 4)
    {
        internalFormat = params.sRGB ? GL_SRGB_ALPHA : GL_RGBA;
        format = GL_RGBA;
    }
    else if(channelCount == 3)
    {
        internalFormat = params.sRGB ? GL_SRGB : GL_RGB;
        format = GL_RGB;
    }
    else if(channelCount == 1)
    {
        internalFormat = GL_RED;
        format = GL_RED;
    }

    glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    if (params.Mipmaps)
        glGenerateMipmap(target);
}

void TextureLoader::SetTextureParams(const Texture::Params& params, unsigned int target)
{
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, params.Mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int repeat = 0;
    switch (params.Repeat)
    {
        case Texture::Clamp:
            repeat = GL_CLAMP_TO_BORDER;
            break;

        case Texture::Extend:
            repeat = GL_CLAMP_TO_EDGE;
            break;

        case Texture::Repeat:
            repeat = GL_REPEAT;
            break;

        case Texture::MirrorRepeat:
            repeat = GL_MIRRORED_REPEAT;
            break;
    }
    glTexParameteri(target, GL_TEXTURE_WRAP_S, repeat);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, repeat);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, repeat);
}
