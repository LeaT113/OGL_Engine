#ifndef OGL_ENGINE_RENDERERCOMPONENT_HPP
#define OGL_ENGINE_RENDERERCOMPONENT_HPP

#include <memory>
#include "Component.hpp"
#include "../Data/MeshData.hpp"
#include "../Data/MaterialData.hpp"
#include "TransformComponent.hpp"
#include "CameraComponent.hpp"


class RendererComponent : public Component
{
public:
	RendererComponent(const Entity *owner, const MeshData &mesh, MaterialData &material);

	void Render(CameraComponent& camera);

private:
	const MeshData &_mesh;
	MaterialData &_material;

    TransformComponent* _transform;
};


#endif
