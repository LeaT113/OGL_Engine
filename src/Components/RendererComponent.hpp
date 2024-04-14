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
    RendererComponent(const Entity& owner, const Mesh* mesh, const Material* material);
    explicit RendererComponent(const Entity& owner);

	const Mesh& GetMesh() const;
    const Material& GetMaterial() const;

    TransformComponent& GetTransform() const;

    const void Render() const;

private:
    friend class Serializer;

    const Mesh* _mesh;
    const Material* _material;

    TransformComponent* _transform;
};


#endif
