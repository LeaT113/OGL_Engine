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

	void EnableEntityIds(bool enable);
	static unsigned int GetEntityId(unsigned int x, unsigned int y);
	void SetHighlightEntity(const Entity* entity);

private:
	static void RenderEntityIds();

    int _width = 1, _height = 1;
	static inline FrameBuffer::RenderFormat RenderFormatColor = FrameBuffer::RenderFormat::RGBA16F;
	static inline FrameBuffer::RenderFormat RenderFormatDepth = FrameBuffer::RenderFormat::Depth24Stencil8;
	static inline FrameBuffer::RenderFormat RenderFormatEntityId = FrameBuffer::RenderFormat::R32UI;
	static inline unsigned int EntityIdDownscaleFactor = 4;
	static inline unsigned int VolumetricDownscaleFactor = 2;


	CameraComponent* _camera = nullptr;
	std::unique_ptr<UniformBuffer> _coreUniformBuffer;
	Handle<Material> _postprocessMaterial;

	// Passes
	std::vector<RendererComponent*> _opaqueRenderers;
	std::vector<RendererComponent*> _transparentRenderers;

	FrameBuffer _renderBuffer;
	Handle<Texture> _opaqueTexture;
	Handle<Texture> _depthTexture;

	FrameBuffer _shadowBuffer;
	const Shader* _shadowShader = nullptr;
	const Shader* _pointShadowShader = nullptr;

	FrameBuffer _entityIdBuffer;
	bool _enableEntityIds = false;
	const Entity* _highlightEntity = nullptr;
	const Shader* _entityIdShader = nullptr;
	const Shader* _entityHighlightShader = nullptr;

	FrameBuffer _fogBuffer;
	Handle<Material> _volumetricFogMaterial;
};


#endif
