#ifndef OGL_ENGINE_RENDERSYSTEM_HPP
#define OGL_ENGINE_RENDERSYSTEM_HPP

#include <vector>
#include "../Components/CameraComponent.hpp"
#include "../Components/RendererComponent.hpp"
#include "../Core/HybridSingleton.hpp"
#include "../OGL/FrameBuffer.hpp"


class UniformBuffer;

class RenderSystem : public HybridSingleton<RenderSystem>
{
public:
	RenderSystem();

	void SetRenderSize(int width, int height);
	void SetRenderCamera(CameraComponent *camera);

	static void RegisterRenderer(RendererComponent* renderer);

    static void Render();

private:
    int _width = 1, _height = 1;
	static inline FrameBuffer::RenderFormat RenderFormatColor = FrameBuffer::RenderFormat::RGBA16F;
	static inline FrameBuffer::RenderFormat RenderFormatDepth = FrameBuffer::RenderFormat::Depth24Stencil8;

	CameraComponent* _camera = nullptr;
	std::unique_ptr<UniformBuffer> _coreUniformBuffer;
	Handle<Material> _postprocessMaterial;

	// Passes
	FrameBuffer _renderBuffer;
	FrameBuffer _shadowBuffer;
	std::vector<RendererComponent*> _opaqueRenderers;
	std::vector<RendererComponent*> _transparentRenderers;
	Handle<Texture> _opaqueTexture;
	Handle<Texture> _depthTexture;
	const Shader* _shadowShader = nullptr;
	const Shader* _pointShadowShader = nullptr;
};


#endif
