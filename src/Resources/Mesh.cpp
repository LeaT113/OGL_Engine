#include "Mesh.hpp"

const VertexArray &Mesh::GetVertexArray() const
{
    return _vertexArray;
}

const std::vector<Submesh> &Mesh::Submeshes() const
{
    return _submeshes;
}
