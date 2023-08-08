#include "RendererComponent.hpp"
#include "../Entity/Entity.hpp"
#include "../Systems/RenderSystem.hpp"


RendererComponent::RendererComponent(const Entity *owner, const MeshData &mesh, MaterialData &material) : _mesh(mesh),
																									_material(material)
{
	_owner = owner;
    _transform = _owner->GetComponent<TransformComponent>();

	RenderSystem::RegisterRenderer(this);
}

void RendererComponent::Render(CameraComponent& camera)
{
    _material.Mat4Property("uMVP") = camera.Projection() * camera.View() * _transform->ModelToWorld();
    _material.Mat4Property("uModelToWorld") = _transform->ModelToWorld();

	_mesh.Bind();
	_material.Bind();
	_mesh.Draw();
}
