#include "Graphics.hpp"


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


void Graphics::RenderMesh(const Mesh &mesh, unsigned int submeshIndex, glm::mat4 modelToWorld,
                          const Material &material, CameraComponent &camera)
{
    Graphics::Bind(mesh.GetVertexArray());

    // Bind shader
    const Shader &shader = material.GetShader();
    Graphics::Bind(shader);

    // Set material values
    material.ApplyValues();

    // Set transformation matrices
    shader.SetTransformations(modelToWorld, camera);

    // Set universal uniforms

    // set pipeline settings like depth write, blending

    // Call draw
    auto submesh = mesh.GetSubmesh(submeshIndex);
    glDrawElements(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset);
}
