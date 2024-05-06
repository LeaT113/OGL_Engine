#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <set>
#include <vector>
#include <GL/glew.h>

#include "IBindable.hpp"
#include "../Core/Handle.hpp"
#include "../Resources/Texture.hpp"

class FrameBuffer : public IBindable
{
public:
    using RenderFormat = Texture::Format;

    FrameBuffer(int width, int height, std::initializer_list<RenderFormat> attachmentFormats);
    ~FrameBuffer() override;

    void Resize(int width, int height);

    const Texture* GetColorTexture(uint32_t index = 0) const;
    const Texture* GetDepthTexture() const;

    void Clear() const;

    GLuint GetBindID() const override;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    const static FrameBuffer None;
private:
    FrameBuffer();

    static inline const std::set<RenderFormat> RenderFormats = {
        RenderFormat::R32UI, RenderFormat::RGB8,
        RenderFormat::RGBA8, RenderFormat::RGBA16F,
        RenderFormat::Depth16, RenderFormat::Depth24Stencil8,
        RenderFormat::Depth32, RenderFormat::Depth32F
    };;

    GLuint _fbo = 0;
    unsigned int _width = 0, _height = 0;
    std::vector<RenderFormat> _colorFormats;
    RenderFormat _depthFormat = RenderFormat::None;

    std::vector<Handle<Texture>> _colorAttachments;
    Handle<Texture> _depthAttachment;
};



#endif
