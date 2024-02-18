#include "Graphics.hpp"

#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "../Resources/Shader.hpp"
#include "UniformBuffer.hpp"
#include "../Resources/Mesh.hpp"
#include "../Resources/Material.hpp"
#include "../Resources/Texture.hpp"
#include "../Components/CameraComponent.hpp"


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

void Graphics::Bind(const Shader &shader)
{
    // TODO Only do if not already bound?
    glUseProgram(shader.GetBindID());
}

void Graphics::Bind(const UniformBuffer& uniformBuffer, unsigned int slot)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, uniformBuffer.GetBindID());
}

void Graphics::Bind(const Texture& texture, unsigned slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.GetBindID());
}


void Graphics::RenderMesh(const Mesh &mesh, unsigned int submeshIndex, const glm::mat4 &modelToWorld,
                          const Material &material, const CameraComponent &camera)
{
    Graphics::Bind(mesh.GetVertexArray());

    // Bind shader
    const Shader &shader = material.GetShader();
    Graphics::Bind(shader);
    shader.BindTextureUnits();

    // Apply material
    material.ApplyValues();
    material.BindTextures();

    // Set transformation matrices
    shader.SetTransformations(modelToWorld, camera.View(), camera.Projection());

    // Set universal uniforms

    // set pipeline settings like depth write, blending

    // Call draw
    auto submesh = mesh.GetSubmesh(submeshIndex);
    glDrawElements(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset);
}