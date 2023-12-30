#ifndef OGL_ENGINE_GRAPHICS_HPP
#define OGL_ENGINE_GRAPHICS_HPP

#include <glm/glm.hpp>
#include "../Resources/Mesh.hpp"
#include "../Resources/Shader.hpp"
#include "../Resources/Material.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"


class Graphics
{
public:
    // Binding
    static void Bind(const VertexBuffer& vertexBuffer);
    static void Bind(const IndexBuffer& indexBuffer);
    static void Bind(const VertexArray& vertexArray);
    static void Bind(const Shader& shader);
    // static void Bind(const Texture& texture, unsigned int slot);

    // Render
    static void RenderMesh(const Mesh &mesh, unsigned int submeshIndex, const glm::mat4 &modelToWorld, const Material &material, const CameraComponent &camera);

    //static void DrawMesh(const MeshData* mesh, );
    //static void DrawMeshInstanced;
};

#endif
