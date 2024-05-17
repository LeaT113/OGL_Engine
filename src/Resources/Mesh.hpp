#ifndef OGL_ENGINE_MESH_HPP
#define OGL_ENGINE_MESH_HPP

#include "Resource.hpp"
#include "../OGL/VertexBuffer.hpp"
#include "../OGL/IndexBuffer.hpp"
#include "../OGL/VertexArray.hpp"


struct Submesh
{
    unsigned int offset;
    int elementCount;
};

class Mesh : public Resource
{
public:
    Mesh(std::string name, Handle<VertexBuffer> vertexBuffer, Handle<IndexBuffer> indexBuffer, Handle<VertexArray> vertexArray, std::vector<glm::vec3> vertices = std::vector<glm::vec3>());

    const VertexArray &GetVertexArray() const;

    const std::vector<glm::vec3> &GetVertices() const;

    // Submeshes
    void AddSubmesh(Submesh submesh);
    const Submesh &GetSubmesh(unsigned int index) const;

private:
    Handle<VertexBuffer> _vertexBuffer;
    Handle<IndexBuffer> _indexBuffer;
    Handle<VertexArray> _vertexArray;
    std::vector<Submesh> _submeshes;

    const std::vector<glm::vec3> _vertices;
};


#endif
