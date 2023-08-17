#include "ModelLoader.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include "../Core/Handle.hpp"
#include "../OGL/VertexBuffer.hpp"
#include "../OGL/IndexBuffer.hpp"
#include "../OGL/VertexArray.hpp"
#include "Mesh.hpp"
#include "../Systems/ResourceDatabase.hpp"


const std::string ModelLoader::ModelsPath = "res/Models/";
unsigned int ModelLoader::PostProcessingFlags = aiProcess_Triangulate |
                                                aiProcess_GenSmoothNormals |
                                                aiProcess_CalcTangentSpace |
                                                aiProcess_JoinIdenticalVertices;

Entity *ModelLoader::LoadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(ModelsPath+path, PostProcessingFlags);

    if(scene == nullptr)
        throw std::runtime_error("ModelLoader::LoadModel failed on " + path);


    const aiMesh* mesh = scene->mMeshes[0];
    const auto name = mesh->mName;

    uint32_t vertexCount = mesh->mNumVertices;


    // Attributes
    std::vector<VertexAttribute> attributes;

    float* positions = reinterpret_cast<float*>(mesh->mVertices);
    attributes.push_back(VertexAttribute::Position);

    float* normals = nullptr;
    if(mesh->HasNormals())
    {
        normals = reinterpret_cast<float*>(mesh->mNormals);
        attributes.push_back(VertexAttribute::Normal);
    }

    float* tangents = nullptr;
    if(mesh->HasTangentsAndBitangents())
    {
        tangents = reinterpret_cast<float*>(mesh->mTangents);
        attributes.push_back(VertexAttribute::Tangent);
    }

    for(int i = 0; i < mesh->GetNumUVChannels(); i++)
    {
        // TODO
    }

    // Indices
    uint32_t triangleCount = mesh->mNumFaces;
    std::vector<unsigned int> indices(triangleCount * 3);
    for (size_t i = 0; i < triangleCount; i++)
    {
        indices[i*3 + 0] = mesh->mFaces[i].mIndices[0];
        indices[i*3 + 1] = mesh->mFaces[i].mIndices[1];
        indices[i*3 + 2] = mesh->mFaces[i].mIndices[2];
    }

    // Buffers
    unsigned int vertexDataSize = 0;
    for(auto attrib : attributes)
        vertexDataSize += vertexCount * GetVertexAttributeInfo(attrib).byteSize;
    auto vertexBuffer = Handle<VertexBuffer>::Make(nullptr, vertexDataSize);
    unsigned int offset = 0;
    vertexBuffer->InsertAttribute(VertexAttribute::Position, offset, positions, vertexCount);
    if(normals)
        vertexBuffer->InsertAttribute(VertexAttribute::Normal, offset, normals, vertexCount);
    if(tangents)
        vertexBuffer->InsertAttribute(VertexAttribute::Tangent, offset, tangents, vertexCount);

    auto indexBuffer = Handle<IndexBuffer>::Make(indices.data(), indices.size() * sizeof(unsigned int));

    auto vertexArray = Handle<VertexArray>::Make(VertexArray::Layout::Packed, vertexCount);
    vertexArray->AddVertexBuffer(*vertexBuffer, attributes);
    vertexArray->SetIndexBuffer(*indexBuffer);

    // Mesh
    // TODO Give mesh to ResourceDatabase with name
    auto m = Handle<Mesh>::Make(vertexBuffer.Pass(), indexBuffer.Pass(), vertexArray.Pass());
    m->AddSubmesh({0, static_cast<int>(triangleCount*3)});
    ResourceDatabase::AddMesh(path, m.Pass());

    return nullptr;
}
