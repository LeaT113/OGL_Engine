#include "RenderSystem.hpp"

#include "ResourceDatabase.hpp"
#include "../OGL/Graphics.hpp"


RenderSystem::RenderSystem()
    : _renderBuffer(0, 0, { FrameBuffer::TextureFormat::RGBA8, FrameBuffer::TextureFormat::Depth24Stencil8 }),
    _opaqueBuffer(0, 0, { FrameBuffer::TextureFormat::RGBA8, FrameBuffer::TextureFormat::Depth24Stencil8 }),
    _reflectionBuffer(0, 0, {FrameBuffer::TextureFormat::RGBA8 }),
    _postprocessMaterial(Handle<Material>::Empty())
{}

void RenderSystem::SetRenderSize(int width, int height)
{
    _width = width;
    _height = height;

    _renderBuffer.Resize(width, height);
    _opaqueBuffer.Resize(width, height);
    _reflectionBuffer.Resize(width, height);

	if(!_camera)
		return;
	_camera->SetWindowDimensions(width, height);
}

void RenderSystem::SetRenderCamera(CameraComponent *camera)
{
	_camera = camera;
    _camera->SetWindowDimensions(_width, _height);
}

void RenderSystem::RegisterRenderer(RendererComponent *renderer)
{
    if (renderer->GetMaterial().GetShader().IsOpaque())
	    Instance()._opaqueRenderers.push_back(renderer);
    else
        Instance()._transparentRenderers.push_back(renderer);
}

void RenderSystem::Render()
{
    // TODO Set universal uniforms like _Time in uniform buffer
    // TODO Set main camera params

    // Opaque pass
    auto& renderBuffer = Instance()._renderBuffer;
    auto& opaqueBuffer = Instance()._opaqueBuffer;
    Graphics::Bind(renderBuffer);
    renderBuffer.Clear();
    for (auto renderer : Instance()._opaqueRenderers)
    {
        Graphics::Render(*renderer, *Instance()._camera);
    }

    // Copy and bind opaque buffer
    Graphics::Blit(renderBuffer, opaqueBuffer);

    // TODO Render reflection texture and generate mipmaps

    // Transparent pass
    // TODO sort transparencies
    for (auto renderer : Instance()._transparentRenderers)
    {
        Graphics::Render(*renderer, *Instance()._camera);
    }

    // Post process
    if (Instance()._postprocessMaterial.Access() == nullptr)
        Instance()._postprocessMaterial = Handle<Material>::Make(*ResourceDatabase::GetShader("PostprocessShader.glsl"), "PostprocessMaterial");
    Graphics::Blit(renderBuffer, FrameBuffer::None, *Instance()._postprocessMaterial);
}
