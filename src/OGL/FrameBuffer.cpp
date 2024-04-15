#include "FrameBuffer.hpp"

#include <stdexcept>

const FrameBuffer FrameBuffer::None = FrameBuffer();

FrameBuffer::FrameBuffer() = default;

FrameBuffer::FrameBuffer(int width, int height, std::initializer_list<TextureFormat> attachmentFormats)
{
    for (auto format : attachmentFormats)
    {
        if (format == TextureFormat::None)
            continue;

        if (format == TextureFormat::Depth24Stencil8 || format == TextureFormat::Depth32)
            _depthFormat = format;
        else
            _colorFormats.emplace_back(format);
    }

    if (width > 0 && height > 0)
        Resize(width, height);
}

FrameBuffer::~FrameBuffer()
{
    if (_fbo)
    {
        glDeleteFramebuffers(1, &_fbo);

        for (auto attachment : _colorAttachments)
            glDeleteTextures(1, &attachment);

        if (_depthAttachment)
            glDeleteTextures(1, &_depthAttachment);
    }
}

void FrameBuffer::Resize(int width, int height)
{
    _width = width;
    _height = height;

    if (_fbo)
    {
        glDeleteFramebuffers(1, &_fbo);

        for (auto attachment : _colorAttachments)
            glDeleteTextures(1, &attachment);
        _colorAttachments.clear();

        if (_depthAttachment)
            glDeleteTextures(1, &_depthAttachment);
        _depthAttachment = 0;
    }

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    for (auto format : _colorFormats)
    {
        GLuint attachment;
        glCreateTextures(GL_TEXTURE_2D, 1, &attachment);
        glBindTexture(GL_TEXTURE_2D, attachment);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        switch (format)
        {
        case TextureFormat::RGB8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            break;

        case TextureFormat::RGBA8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            break;

        case TextureFormat::RGBA16F:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
            break;
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _colorAttachments.size(), GL_TEXTURE_2D, attachment, 0);
        _colorAttachments.emplace_back(attachment);
    }

    if (_depthFormat != TextureFormat::None)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &_depthAttachment);
        glBindTexture(GL_TEXTURE_2D, _depthAttachment);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        switch (_depthFormat)
        {
        case TextureFormat::Depth24Stencil8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthAttachment, 0);
            break;

        case TextureFormat::Depth32:
            // TODO Depth32 glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            break;
        }
    }

    if (_colorAttachments.size() >= 1)
    {
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(_colorAttachments.size(), buffers);
    }
    else
    {
        glDrawBuffer(GL_NONE);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("FrameBuffer::Resize Incomplete framebuffer");
}

GLuint FrameBuffer::GetColorTexture(uint32_t index) const
{
    if (index > _colorAttachments.size())
        throw std::runtime_error("FrameBuffer::GetColorTexture Index out of range");

    return _colorAttachments[index];
}

GLuint FrameBuffer::GetDepthTexture() const
{
    return _depthAttachment;
}

void FrameBuffer::Clear() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    unsigned int clearMask = 0;

    if (!_colorAttachments.empty())
        // TODO glDraw to turn on clearing?
        clearMask |= GL_COLOR_BUFFER_BIT;

    if (_depthAttachment != 0)
    {
        glDepthMask(GL_TRUE);
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }

    glClear(clearMask);
}

GLuint FrameBuffer::GetBindID() const
{
    return _fbo;
}

unsigned int FrameBuffer::GetWidth() const
{
    return _width;
}

unsigned int FrameBuffer::GetHeight() const
{
    return _height;
}
