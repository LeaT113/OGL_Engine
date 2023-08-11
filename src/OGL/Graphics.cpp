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


void Graphics::RenderMesh(const Mesh &mesh, unsigned int submeshIndex, glm::mat4 modelToWorld,
                          const MaterialData &material)
{
    Graphics::Bind(mesh.GetVertexArray());

    // Bind shader
    // communicate vertex data layout
    // set uniforms from material
    // pass transformation matrices and other universal uniforms not stored in material
    // set pipeline settings like depth write, blending

    // Call draw
    auto submesh = mesh.GetSubmesh(submeshIndex);
    glDrawElements(GL_TRIANGLES, submesh.elementCount, GL_UNSIGNED_INT, (void*)(uintptr_t)submesh.offset);
}
