#include "Graphics.hpp"

#include "FrameBuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "../Resources/Shader.hpp"
#include "UniformBuffer.hpp"
#include "../Resources/Mesh.hpp"
#include "../Resources/Material.hpp"
#include "../Resources/Texture.hpp"
#include "../Components/CameraComponent.hpp"
#include "../IO/MeshFactory.hpp"
#include "../Systems/ResourceDatabase.hpp"


void Graphics::Bind(const VertexBuffer &vertexBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.GetBindID());
}

void Graphics::Bind(const IndexBuffer &indexBuffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.GetBindID());
}

void Graphics::Bind(const VertexArray &vertexArray)
{
    glBindVertexArray(vertexArray.GetBindID());
}

void Graphics::Bind(const Mesh& mesh)
{
    Bind(mesh.GetVertexArray());
}

void Graphics::Bind(const Shader &shader)
{
    // TODO Only do if not already bound?
    glUseProgram(shader.GetBindID());
    shader.BindTextureUnits();
}

void Graphics::Bind(const UniformBuffer& uniformBuffer, unsigned int slot)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, uniformBuffer.GetBindID());
}

void Graphics::Bind(const Texture& texture, unsigned slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(static_cast<int>(texture.GetType()), texture.GetBindID());
}

void Graphics::Bind(const FrameBuffer& frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.GetBindID());
}


void Graphics::RenderMesh(const Mesh &mesh, unsigned int submeshIndex, const glm::mat4 &modelToWorld,
                          const Material &material, const CameraComponent &camera)
{
    Graphics::Bind(mesh.GetVertexArray());

    // Bind shader
    const Shader &shader = material.GetShader();
    Graphics::Bind(shader);

    // Apply material
    material.ApplyValues();
    material.BindTextures();

    // Set transformation matrices
    shader.SetTransformations(modelToWorld, camera.View(), camera.Projection());

    // Set universal uniforms

    // Set pipeline settings like depth write, blending
    shader.SetPipelineState();

    // Call draw
    auto submesh = mesh.GetSubmesh(submeshIndex);
    glDrawElements(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset);
}

void Graphics::Render(const RendererComponent& renderer, const CameraComponent& camera)
{
    // TODO Set _ScreenSize in px, _WorldSpaceCameraPos
    // Or not here and rely on pipeline?

    // TODO Iterate through submeshes
    RenderMesh(renderer.GetMesh(), 0, renderer.GetTransform()->ModelToWorld(), renderer.GetMaterial(), camera);
}

void Graphics::Blit(const FrameBuffer& source, const FrameBuffer& destination)
{
    glBlitNamedFramebuffer(source.GetBindID(), destination.GetBindID(), 0, 0, source.GetWidth(), source.GetHeight(), 0, 0, destination.GetWidth(), destination.GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
}

void Graphics::Blit(const FrameBuffer& source, const FrameBuffer& destination, const Material& material)
{
    const static Mesh* quad = ResourceDatabase::GetMesh("Quad");

    Graphics::Bind(*quad);

    // Bind shader
    const Shader &shader = material.GetShader();
    Graphics::Bind(shader);

    // Apply material
    material.ApplyValues();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, source.GetColorTexture());

    // Set pipeline settings like depth write, blending
    shader.SetPipelineState();

    Graphics::Bind(destination);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, (void*)0);
}
