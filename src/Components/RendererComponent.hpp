#ifndef OGL_ENGINE_RENDERERCOMPONENT_HPP
#define OGL_ENGINE_RENDERERCOMPONENT_HPP

#include "Component.hpp"
#include "../Resources/Mesh.hpp"
#include "../Resources/Material.hpp"


class RendererComponent : public Component
{
public:
    RendererComponent(const Entity& owner, const Mesh* mesh, const Material* material);
    explicit RendererComponent(const Entity& owner);

	const Mesh& GetMesh() const;
    const Material& GetMaterial() const;

    // Instancing
    bool IsInstanced() const;
    const std::vector<glm::mat4>& GetInstancingTransforms() const;
    void SetInstancingTransforms(std::vector<glm::mat4> transforms);
    const VertexBuffer* GetInstancingTransformsBuffer() const;

private:
    friend class Serializer;

    const Mesh* _mesh = nullptr;
    const Material* _material = nullptr;

    std::vector<glm::mat4> _instancingTransforms;
    Handle<VertexBuffer> _instancingTransformsBuffer;
};


#endif
