#include "MeshFactory.hpp"

Handle<Mesh> MeshFactory::CreateQuad()
{
    static float positions[4 * 3] =
        {
            -1, -1, 0,
            1, -1, 0,
            1, 1, 0,
            -1, 1, 0
        };

    static float uvs[4 * 2] =
        {
            0, 0,
            1, 0,
            1, 1,
            0, 1
        };

    static unsigned int indicies[2 * 3] =
        {
            0, 1, 3,
            1, 2, 3
        };

    auto vertexBuffer =  Handle<VertexBuffer>::Make((GetVertexAttributeInfo(VertexAttribute::Position).byteSize + GetVertexAttributeInfo(VertexAttribute::TexCoord0).byteSize) * 4);
    unsigned int offset = 0;
    vertexBuffer->InsertAttribute(VertexAttribute::Position, positions, 4);
    vertexBuffer->InsertAttribute(VertexAttribute::TexCoord0, uvs, 4);
    auto indexBuffer = Handle<IndexBuffer>::Make(indicies, 2 * 3 * sizeof(unsigned int));
    auto vertexArray = Handle<VertexArray>::Make(VertexArray::Layout::Packed, 4);
    vertexArray->AddVertexBuffer(*vertexBuffer, std::vector{ VertexAttribute::Position, VertexAttribute::TexCoord0 });
    vertexArray->SetIndexBuffer(*indexBuffer);

    auto mesh = Handle<Mesh>::Make("Quad", vertexBuffer.Pass(), indexBuffer.Pass(), vertexArray.Pass());
    mesh->AddSubmesh(Submesh{0, 4 * 3});

    return mesh;
}