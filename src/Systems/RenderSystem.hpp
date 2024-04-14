#ifndef OGL_ENGINE_RENDERSYSTEM_HPP
#define OGL_ENGINE_RENDERSYSTEM_HPP

#include <vector>
#include "../Components/CameraComponent.hpp"
#include "../Components/RendererComponent.hpp"
#include "../Core/HybridSingleton.hpp"
#include "../OGL/FrameBuffer.hpp"


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

	// Passes
	FrameBuffer _renderBuffer;
	FrameBuffer _opaqueBuffer;
	std::vector<RendererComponent*> _opaqueRenderers;

	FrameBuffer _reflectionBuffer;
	std::vector<RendererComponent*> _transparentRenderers;

	Handle<Material> _postprocessMaterial;
};


#endif
