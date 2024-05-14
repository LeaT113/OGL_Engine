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
    const auto imageData = stbi_load(std::format("{}{}", TexturesPath, name).c_str(), &width, &height, &channelCount, 0);
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
        .name = name.substr(0, name.find_first_of('.')), .data = imageData, .settings = settings
    });
    stbi_image_free(imageData);

    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

Handle<Texture> TextureLoader::LoadCubemap(const std::string& name, const Texture::Settings& settings)
{
    stbi_set_flip_vertically_on_load(0);

    auto tex = Handle<Texture>::Empty();

    auto dotPos = name.find_first_of('.');
    auto name1 = name.substr(0, dotPos);
    auto ext = name.substr(dotPos+1);

    static std::string suffixes[6] = {"px", "nx", "py", "ny", "pz", "nz"};
    for (int i = 0; i < 6; i++)
    {
        int width, height, channelCount;
        const auto imageData = stbi_loadf(std::format("{}{}_{}.{}", TexturesPath, name1, suffixes[i], ext).c_str(), &width, &height, &channelCount, 0);
        if (!imageData)
            throw std::runtime_error("TextureLoader Failed to load image");

        if (tex.Access() == nullptr)
            tex = Handle<Texture>::Make(Texture::Type::TexCubemap, Texture::Format::RGB16F, Texture::Params {
                .width = static_cast<unsigned int>(width), .height = static_cast<unsigned int>(height),
                .name = name1, .settings = settings
            });

        tex->Fill(imageData, i);
        stbi_image_free(imageData);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return tex;
}
