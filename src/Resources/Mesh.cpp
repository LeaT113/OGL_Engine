#include "Mesh.hpp"


Mesh::Mesh(Handle<VertexBuffer> vertexBuffer, Handle<IndexBuffer> indexBuffer, Handle<VertexArray> vertexArray) : _vertexBuffer(
        vertexBuffer.Pass()),
                                                                                                                  _indexBuffer(
                                                                                                                          indexBuffer.Pass()),
                                                                                                                  _vertexArray(
                                                                                                                          vertexArray.Pass())
{

}


const VertexArray &Mesh::GetVertexArray() const
{
    return *_vertexArray;
}

const Submesh &Mesh::GetSubmesh(unsigned int index) const
{
    return _submeshes[index];
}

void Mesh::AddSubmesh(Submesh submesh)
{
    _submeshes.push_back(submesh);
}
