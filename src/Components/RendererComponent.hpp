#ifndef OGL_ENGINE_RENDERERCOMPONENT_HPP
#define OGL_ENGINE_RENDERERCOMPONENT_HPP

#include <memory>
#include "Component.hpp"
#include "../Data/MaterialData.hpp"
#include "TransformComponent.hpp"
#include "CameraComponent.hpp"
#include "../Resources/Mesh.hpp"


class RendererComponent : public Component
{
public:
    RendererComponent(const Entity *owner, const Mesh* mesh, MaterialData &material);

	void Render(CameraComponent& camera);

private:
    const Mesh* _mesh;

	MaterialData &_material;

    TransformComponent* _transform;
};


#endif
