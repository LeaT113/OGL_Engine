#include "RenderSystem.hpp"

#include <algorithm>
#include <glm/gtx/norm.hpp>

#include "ResourceDatabase.hpp"
#include "TimeKeeper.hpp"
#include "../OGL/Graphics.hpp"
#include "../OGL/UniformBuffer.hpp"


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
    // Set core uniforms
    if (Instance()._coreUniformBuffer == nullptr)
    {
        Instance()._coreUniformBuffer = std::make_unique<UniformBuffer>(sizeof(float) + sizeof(glm::vec2)*2);
        Graphics::Bind(*Instance()._coreUniformBuffer, 1);
    }
    glm::vec2 screenSize(Instance()._width, Instance()._height);
    Instance()._coreUniformBuffer->SetData(0, sizeof(glm::vec2), &screenSize);
    auto cameraNearFar = Instance()._camera->GetNearFar();
    Instance()._coreUniformBuffer->SetData(sizeof(glm::vec2), sizeof(glm::vec2), &cameraNearFar);
    auto time = static_cast<float>(TimeKeeper::TimeSinceStartup());
    Instance()._coreUniformBuffer->SetData(sizeof(glm::vec2) * 2, sizeof(float), &time);

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
    opaqueBuffer.Clear();
    Graphics::Blit(renderBuffer, opaqueBuffer);

    // Transparent pass
    Graphics::Bind(renderBuffer);
    std::ranges::sort(Instance()._transparentRenderers, [](RendererComponent* a, RendererComponent* b)
    {
        auto dist1 = length2(a->GetTransform()->Position() - Instance()._camera->GetTransform()->Position());
        auto dist2 = length2(b->GetTransform()->Position() - Instance()._camera->GetTransform()->Position());
        return dist1 > dist2;
    });
    for (auto renderer : Instance()._transparentRenderers)
    {
        auto& shader = renderer->GetMaterial().GetShader();
        shader.SetTextures(opaqueBuffer.GetColorTexture(), opaqueBuffer.GetDepthTexture(), ResourceDatabase::GetTexture("Skybox/Night")->GetBindID());

        Graphics::Render(*renderer, *Instance()._camera);
    }

    // Post process
    if (Instance()._postprocessMaterial.Access() == nullptr)
        Instance()._postprocessMaterial = Handle<Material>::Make(*ResourceDatabase::GetShader("PostprocessShader.glsl"), "PostprocessMaterial");
    Graphics::Blit(renderBuffer, FrameBuffer::None, *Instance()._postprocessMaterial);
}
