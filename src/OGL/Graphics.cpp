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
#include "../Components/TransformComponent.hpp"
#include "../Systems/LightingSystem.hpp"
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
                          const Material &material, const CameraComponent &camera, unsigned int instanceCount)
{
    Bind(mesh.GetVertexArray());


    // Bind shader
    const Shader &shader = material.GetShader();
    Bind(shader);
    LightingSystem::BindShadowMaps(shader);

    // Apply material
    material.ApplyValues();
    material.BindTextures();

    // Set transformation matrices
    shader.SetTransformations(modelToWorld, camera.View(), camera.Projection());

    // Set pipeline settings like depth write, blending
    shader.SetPipelineState();

    // Call draw
    auto submesh = mesh.GetSubmesh(submeshIndex);
    if (instanceCount)
    {
        Shader::SetBool(shader.GetUniformLocation("_Instanced"), true);
        glDrawElementsInstanced(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset, static_cast<int>(instanceCount));
    }
    else
    {
        Shader::SetBool(shader.GetUniformLocation("_Instanced"), false);
        glDrawElements(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset);
    }
}

void Graphics::Render(const RendererComponent& renderer, const CameraComponent& camera)
{
    // TODO Set _ScreenSize in px, _WorldSpaceCameraPos
    // Or not here and rely on pipeline?

    // TODO Iterate through submeshes
    RenderMesh(renderer.GetMesh(), 0, renderer.GetTransform()->ModelToWorld(), renderer.GetMaterial(), camera, renderer.GetInstancingTransforms().size());
}

void Graphics::RenderWithShader(const RendererComponent& renderer, const CameraComponent& camera, const Shader& shader)
{
    // TODO Rework
    Bind(renderer.GetMesh().GetVertexArray());

    // Bind shader
    Bind(shader);

    // Set transformation matrices
    shader.SetTransformations(renderer.GetTransform()->ModelToWorld(), camera.View(), camera.Projection());

    // Set pipeline settings like depth write, blending
    shader.SetPipelineState();

    // Call draw
    auto submesh = renderer.GetMesh().GetSubmesh(0);
    if (renderer.IsInstanced())
    {
        Shader::SetBool(shader.GetUniformLocation("_Instanced"), true);
        glDrawElementsInstanced(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset, renderer.GetInstancingTransforms().size());
    }
    else
    {
        Shader::SetBool(shader.GetUniformLocation("_Instanced"), false);
        glDrawElements(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset);
    }
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
    glActiveTexture(GL_TEXTURE0 + shader.GetUniformLocation("SourceTex"));
    glBindTexture(GL_TEXTURE_2D, source.GetColorTexture()->GetBindID());

    // Set pipeline settings like depth write, blending
    shader.SetPipelineState();

    Graphics::Bind(destination);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, (void*)0);
}
