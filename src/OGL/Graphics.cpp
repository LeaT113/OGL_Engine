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
    // call draw

    Graphics::Bind(VertexBuffer::None);
}
