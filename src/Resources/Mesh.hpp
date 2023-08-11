#ifndef OGL_ENGINE_MESH_HPP
#define OGL_ENGINE_MESH_HPP

#include <vector>
#include "../Core/Handle.hpp"
#include "../OGL/VertexBuffer.hpp"
#include "../OGL/IndexBuffer.hpp"
#include "../OGL/VertexArray.hpp"


struct Submesh
{
    unsigned int offset;
    int elementCount;
};

class Mesh
{
public:
    Mesh(Handle<VertexBuffer> vertexBuffer, Handle<IndexBuffer> indexBuffer, Handle<VertexArray> vertexArray);

    const VertexArray &GetVertexArray() const;

    // Submeshes
    void AddSubmesh(Submesh submesh);
    const Submesh &GetSubmesh(unsigned int index) const;

private:
    Handle<VertexBuffer> _vertexBuffer;
    Handle<IndexBuffer> _indexBuffer;
    Handle<VertexArray> _vertexArray;
    std::vector<Submesh> _submeshes;
};


#endif
