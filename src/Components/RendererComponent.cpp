#include "RendererComponent.hpp"
#include "../Scene/Entity.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../OGL/Graphics.hpp"
#include "../Systems/ResourceDatabase.hpp"


RendererComponent::RendererComponent(const Entity& owner, const Mesh* mesh, const Material* material)
	: Component(owner), _mesh(mesh), _material(material)
{
    _transform = _owner->GetComponent<TransformComponent>();

	RenderSystem::RegisterRenderer(this);
}

RendererComponent::RendererComponent(const Entity& owner) : Component(owner)
{
	_transform = _owner->GetComponent<TransformComponent>();

	RenderSystem::RegisterRenderer(this);
}

void RendererComponent::Render(const CameraComponent &camera) const
{
	Graphics::RenderMesh(*_mesh, 0, _transform->ModelToWorld(), *_material, camera);
}
