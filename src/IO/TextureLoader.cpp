#include "TextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>
#include "../Resources/Texture.hpp"

const std::string TextureLoader::TexturesPath = "res/Textures/";

Handle<Texture> TextureLoader::LoadTexture(const std::string& path)
{
    stbi_set_flip_vertically_on_load(1);

    int width, height, channelCount;
    const auto imageData = stbi_load((TexturesPath + path).c_str(), &width, &height, &channelCount, 0);
    if (imageData == nullptr)
        return Handle<Texture>::Empty();

    GLenum format = 0;
    if(channelCount == 4)
        format = GL_RGBA;
    else if(channelCount == 3)
        format = GL_RGB;
    else if(channelCount == 1)
        format = GL_ALPHA;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // TODO Dynamic
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    if (true) // TODO Option
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Handle<Texture>::Make(texture, path);
}
