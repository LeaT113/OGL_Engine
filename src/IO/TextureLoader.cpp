#include "TextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <format>
#include <stb_image.h>
#include <GL/glew.h>
#include "../Resources/Texture.hpp"

const std::string TextureLoader::TexturesPath = "res/Textures/";

Handle<Texture> TextureLoader::LoadTexture2D(const std::string &name, const Texture::Settings& settings)
{
    stbi_set_flip_vertically_on_load(1);

    int width, height, channelCount;
    const auto imageData = stbi_load(std::format("{}{}.png", TexturesPath, name).c_str(), &width, &height, &channelCount, 0);
    if (!imageData)
        throw std::runtime_error("TextureLoader Failed to load image");

    Texture::Format format;
    if (channelCount == 1)
        format = Texture::Format::R8;
    else if (channelCount == 3)
        format = Texture::Format::RGB8;
    else if (channelCount == 4)
        format = Texture::Format::RGBA8;

    auto tex = Handle<Texture>::Make(Texture::Type::Tex2D, format, Texture::Params {
        .width = static_cast<unsigned int>(width), .height = static_cast<unsigned int>(height),
        .name = name, .data = imageData, .settings = settings
    });

    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

Handle<Texture> TextureLoader::LoadCubemap(const std::string& name, const Texture::Settings& settings)
{
    stbi_set_flip_vertically_on_load(0);

    auto tex = Handle<Texture>::Empty();

    static std::string suffixes[6] = {"px", "nx", "py", "ny", "pz", "nz"};
    for (int i = 0; i < 6; i++)
    {
        int width, height, channelCount;
        const auto imageData = stbi_load(std::format("{}{}_{}.png", TexturesPath, name, suffixes[i]).c_str(), &width, &height, &channelCount, 0);
        if (!imageData)
            throw std::runtime_error("TextureLoader Failed to load image");

        if (tex.Access() == nullptr)
            tex = Handle<Texture>::Make(Texture::Type::TexCubemap, Texture::Format::RGB8, Texture::Params {
                .width = static_cast<unsigned int>(width), .height = static_cast<unsigned int>(height),
                .name = name, .settings = settings
            });

        tex->Fill(imageData, i);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return tex;
}
