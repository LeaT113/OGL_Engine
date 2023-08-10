#ifndef OGL_ENGINE_MESH_HPP
#define OGL_ENGINE_MESH_HPP

#include <vector>
#include "../OGL/VertexBuffer.hpp"
#include "../OGL/IndexBuffer.hpp"
#include "../OGL/VertexArray.hpp"


struct Submesh
{
    unsigned int start;
    unsigned int count;
};

class Mesh
{
public:
    const VertexArray &GetVertexArray() const;

    const std::vector<Submesh>& Submeshes() const;

private:
    VertexBuffer _vertexBuffer;
    IndexBuffer _indexBuffer;
    VertexArray _vertexArray;
    std::vector<Submesh> _submeshes;
};


#endif
