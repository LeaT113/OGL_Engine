#include "Mesh.hpp"

Mesh::Mesh(std::string name, Handle<VertexBuffer> vertexBuffer, Handle<IndexBuffer> indexBuffer, Handle<VertexArray> vertexArray, std::vector<glm::vec3> vertices)
    : Resource(std::move(name)), _vertexBuffer(vertexBuffer.Pass()), _indexBuffer(indexBuffer.Pass()), _vertexArray(vertexArray.Pass()), _vertices(std::move(vertices))
{}


const VertexArray &Mesh::GetVertexArray() const
{
    return *_vertexArray;
}

const std::vector<glm::vec3> & Mesh::GetVertices() const
{
    return _vertices;
}

const Submesh &Mesh::GetSubmesh(unsigned int index) const
{
    return _submeshes[index];
}

void Mesh::AddSubmesh(Submesh submesh)
{
    _submeshes.push_back(submesh);
}
