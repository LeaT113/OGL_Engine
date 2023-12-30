#ifndef OGL_ENGINE_RENDERERCOMPONENT_HPP
#define OGL_ENGINE_RENDERERCOMPONENT_HPP

#include <memory>
#include "Component.hpp"
#include "TransformComponent.hpp"
#include "CameraComponent.hpp"
#include "../Resources/Mesh.hpp"
#include "../Resources/Material.hpp"


class RendererComponent : public Component
{
public:
    RendererComponent(const Entity *owner, const Mesh* mesh, Material &material);

	void Render(const CameraComponent &camera) const;

private:
    const Mesh* _mesh;

	Material &_material;

    TransformComponent* _transform;
};


#endif
