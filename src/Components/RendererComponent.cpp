#include "RendererComponent.hpp"
#include "../Entity/Entity.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../OGL/Graphics.hpp"


RendererComponent::RendererComponent(const Entity *owner, const Mesh* mesh, MaterialData &material) : _mesh(mesh),
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

    Graphics::Bind(_mesh->GetVertexArray());
	_material.Bind();
	Graphics::RenderMesh(*_mesh, 0, _transform->ModelToWorld(), _material);
}
