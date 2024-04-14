#ifndef OGL_ENGINE_GRAPHICS_HPP
#define OGL_ENGINE_GRAPHICS_HPP
#include <glm/fwd.hpp>

#include "../Components/RendererComponent.hpp"

class FrameBuffer;
class VertexBuffer;
class IndexBuffer;
class VertexArray;
class Shader;
class UniformBuffer;
class Texture;
class Mesh;
class Material;
class CameraComponent;
class RendererComponent;


class Graphics
{
public:
    // Binding
    static void Bind(const VertexBuffer &vertexBuffer);
    static void Bind(const IndexBuffer &indexBuffer);
    static void Bind(const VertexArray &vertexArray);
    static void Bind(const Mesh &mesh);
    static void Bind(const Shader &shader);
    static void Bind(const UniformBuffer &uniformBuffer, unsigned int slot);
    static void Bind(const Texture &texture, unsigned int slot);
    static void Bind(const FrameBuffer &frameBuffer);

    // Render
    static void RenderMesh(const Mesh &mesh, unsigned int submeshIndex, const glm::mat4 &modelToWorld, const Material &material, const CameraComponent &camera);
    static void Render(const RendererComponent& renderer, const CameraComponent& camera);
    static void Blit(const FrameBuffer& source, const FrameBuffer& destination);
    static void Blit(const FrameBuffer& source, const FrameBuffer& destination, const Material& material);
    //static void DrawMesh(const MeshData* mesh, );
    //static void DrawMeshInstanced;
};

#endif
