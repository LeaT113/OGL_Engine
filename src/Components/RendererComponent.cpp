#include "RendererComponent.hpp"
#include "../Entity/Entity.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../OGL/Graphics.hpp"


RendererComponent::RendererComponent(const Entity *owner, const Mesh* mesh, Material &material) : _mesh(mesh),
																									_material(material)
{
	_owner = owner;
    _transform = _owner->GetComponent<TransformComponent>();

	RenderSystem::RegisterRenderer(this);
}

void RendererComponent::Render(const CameraComponent &camera) const
{
	Graphics::RenderMesh(*_mesh, 0, _transform->ModelToWorld(), _material, camera);
}
