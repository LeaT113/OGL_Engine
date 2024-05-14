#include "FrameBuffer.hpp"

#include <stdexcept>

const FrameBuffer FrameBuffer::None = FrameBuffer();

FrameBuffer::FrameBuffer() = default;

FrameBuffer::FrameBuffer(int width, int height, std::initializer_list<RenderFormat> attachmentFormats)
{
    for (auto format : attachmentFormats)
    {
        if (!RenderFormats.contains(format))
            throw std::runtime_error("FrameBuffer Invalid format: " + static_cast<int>(format));

        if (Texture::IsDepthFormat(format))
        {
            if (_depthFormat != RenderFormat::None)
                throw std::runtime_error("FrameBuffer Multiple depth formats specified");
            _depthFormat = format;
        }
        else
        {
            if (_colorFormats.size() >= 4)
                throw std::runtime_error("FrameBuffer Specified more than 4 color attachements (max is 4)");
            _colorFormats.push_back(format);
        }
    }

    if (width > 0 && height > 0)
        Resize(width, height);
}

FrameBuffer::~FrameBuffer()
{
    if (_fbo)
        glDeleteFramebuffers(1, &_fbo);
}

void FrameBuffer::Resize(int width, int height)
{
    _width = width;
    _height = height;

    if (_fbo)
    {
        glDeleteFramebuffers(1, &_fbo);

        _colorAttachments.clear();
        _depthAttachment = Handle<Texture>::Empty();
    }

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    for (auto format : _colorFormats)
    {
        _colorAttachments.emplace_back(Handle<Texture>::Make(Texture::Type::Tex2D, format, Texture::Params {
            .width = _width, .height = _height,
            .settings = { .mipmaps = false, .sRGB =  false, .tiling = Texture::Tiling::Extend }
        }));

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _colorAttachments.size() - 1, GL_TEXTURE_2D, _colorAttachments.back()->GetBindID(), 0);
    }

    if (_depthFormat != RenderFormat::None)
    {
        _depthAttachment = Handle<Texture>::Make(Texture::Type::Tex2D, _depthFormat, Texture::Params {
            .width = _width, .height = _height,
            .settings = { .mipmaps = false, .sRGB =  false, .tiling = Texture::Tiling::Extend }
        });

        unsigned int attachmentType;
        switch (_depthFormat)
        {
        case RenderFormat::Depth16:
        case RenderFormat::Depth32:
        case RenderFormat::Depth32F:
            attachmentType = GL_DEPTH_ATTACHMENT;
            break;

        case RenderFormat::Depth24Stencil8:
            attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
            break;

        default:
                throw std::runtime_error("FrameBuffer Unknown depth format");
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, _depthAttachment->GetBindID(), 0);
    }

    if (!_colorAttachments.empty())
    {
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(static_cast<int>(_colorAttachments.size()), buffers);
    }
    else
    {
        glDrawBuffer(GL_NONE);
    }
}

Texture *FrameBuffer::GetColorTexture(uint32_t index) const
{
    if (index > _colorAttachments.size())
        throw std::runtime_error("FrameBuffer::GetColorTexture Index out of range");

    return _colorAttachments[index].Access();
}

const Texture* FrameBuffer::GetDepthTexture() const
{
    return _depthAttachment.Access();
}

void FrameBuffer::Clear() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    unsigned int clearMask = 0;
    if (!_colorAttachments.empty())
        // TODO glDraw to turn on clearing?
        clearMask |= GL_COLOR_BUFFER_BIT;

    if (_depthAttachment.Access() != nullptr)
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
