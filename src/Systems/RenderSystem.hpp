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

	CameraComponent* _camera = nullptr;
	std::unique_ptr<UniformBuffer> _coreUniformBuffer;
	Handle<Material> _postprocessMaterial;

	// Passes
	FrameBuffer _renderBuffer;
	FrameBuffer _opaqueBuffer;
	FrameBuffer _shadowBuffer;
	std::vector<RendererComponent*> _opaqueRenderers;
	std::vector<RendererComponent*> _transparentRenderers;
	const Shader* _shadowShader = nullptr;
	const Shader* _pointShadowShader = nullptr;
};


#endif
