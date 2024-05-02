#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <vector>
#include <GL/glew.h>

#include "IBindable.hpp"


class FrameBuffer : public IBindable
{
public:
    enum class TextureFormat
    {
        None = 0,

        RGB8,
        RGBA8,
        RGBA16F,

        Depth16,
        Depth24Stencil8,
        Depth32F,
    };

    FrameBuffer(int width, int height, std::initializer_list<TextureFormat> attachmentFormats);
    ~FrameBuffer() override;

    void Resize(int width, int height);

    GLuint GetColorTexture(uint32_t index = 0) const;
    GLuint GetDepthTexture() const;

    void Clear() const;

    GLuint GetBindID() const override;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    const static FrameBuffer None;
private:
    FrameBuffer();

    GLuint _fbo = 0;
    unsigned int _width = 0, _height = 0;
    std::vector<TextureFormat> _colorFormats;
    TextureFormat _depthFormat = TextureFormat::None;
    std::vector<GLuint> _colorAttachments;
    GLuint _depthAttachment = 0;
};



#endif
