#include "RenderSystem.hpp"


void RenderSystem::SetRenderSize(int width, int height)
{
    _width = width;
    _height = height;

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
	Instance()._renderers.push_back(renderer);
}

void RenderSystem::Render()
{
    for (auto renderer : Instance()._renderers)
    {
        renderer->Render(*Instance()._camera);
    }
}
