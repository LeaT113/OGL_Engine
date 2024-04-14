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
}

const Mesh& RendererComponent::GetMesh() const
{
	return *_mesh;
}

const Material& RendererComponent::GetMaterial() const
{
	return *_material;
}

TransformComponent& RendererComponent::GetTransform() const
{
	return *_transform;
}
