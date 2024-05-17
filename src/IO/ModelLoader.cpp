#include "ModelLoader.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "../OGL/VertexBuffer.hpp"
#include "../OGL/IndexBuffer.hpp"
#include "../OGL/VertexArray.hpp"
#include "../Resources/Mesh.hpp"

const std::string ModelLoader::ModelsPath = "res/Models/";
unsigned int ModelLoader::PostProcessingFlags = aiProcess_Triangulate |
                                                aiProcess_GenSmoothNormals |
                                                aiProcess_CalcTangentSpace |
                                                aiProcess_JoinIdenticalVertices;

Handle<Mesh> ModelLoader::LoadModel(const std::string& path, bool keepOnCpu)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(ModelsPath+path, PostProcessingFlags);

    if(scene == nullptr)
        throw std::runtime_error("ModelLoader::LoadModel failed on " + path);


    const aiMesh* aiMesh = scene->mMeshes[0];
    const auto name = aiMesh->mName;

    uint32_t vertexCount = aiMesh->mNumVertices;


    // Attributes
    std::vector<VertexAttribute> attributes;

    float* positions = reinterpret_cast<float*>(aiMesh->mVertices);
    attributes.push_back(VertexAttribute::Position);

    float* normals = nullptr;
    if(aiMesh->HasNormals())
    {
        normals = reinterpret_cast<float*>(aiMesh->mNormals);
        attributes.push_back(VertexAttribute::Normal);
    }

    float* tangents = nullptr;
    if(aiMesh->HasTangentsAndBitangents())
    {
        tangents = reinterpret_cast<float*>(aiMesh->mTangents);
        attributes.push_back(VertexAttribute::Tangent);
    }

    std::vector<std::vector<float>> UVs;
    for(int i = 0; i < aiMesh->GetNumUVChannels(); i++)
    {
        UVs.emplace_back();
        UVs[i].reserve((size_t)vertexCount * 2);

        for (size_t j = 0; j < vertexCount; j++)
        {
            aiVector3D vector = (aiMesh->mTextureCoords[i])[j];
            UVs[i].push_back(vector.x);
            UVs[i].push_back(vector.y);
        }

        attributes.push_back(static_cast<VertexAttribute>(static_cast<int>(VertexAttribute::TexCoord0) + i));
    }

    // Indices
    uint32_t triangleCount = aiMesh->mNumFaces;
    std::vector<unsigned int> indices(triangleCount * 3);
    for (size_t i = 0; i < triangleCount; i++)
    {
        indices[i*3 + 0] = aiMesh->mFaces[i].mIndices[0];
        indices[i*3 + 1] = aiMesh->mFaces[i].mIndices[1];
        indices[i*3 + 2] = aiMesh->mFaces[i].mIndices[2];
    }

    // Buffers
    unsigned int vertexDataSize = 0;
    for(auto attrib : attributes)
        vertexDataSize += vertexCount * GetVertexAttributeInfo(attrib).byteSize;
    auto vertexBuffer = Handle<VertexBuffer>::Make(vertexDataSize);
    unsigned int offset = 0;
    vertexBuffer->InsertAttribute(VertexAttribute::Position, positions, vertexCount);
    if(normals)
        vertexBuffer->InsertAttribute(VertexAttribute::Normal, normals, vertexCount);
    if(tangents)
        vertexBuffer->InsertAttribute(VertexAttribute::Tangent, tangents, vertexCount);
    for (int i = 0; i < UVs.size(); i++)
    {
        vertexBuffer->InsertAttribute(static_cast<VertexAttribute>(static_cast<int>(VertexAttribute::TexCoord0) + i), UVs[i].data(), vertexCount);
    }

    auto indexBuffer = Handle<IndexBuffer>::Make(indices);

    auto vertexArray = Handle<VertexArray>::Make(VertexArray::Layout::Packed, vertexCount);
    vertexArray->AddVertexBuffer(*vertexBuffer, attributes);
    vertexArray->SetIndexBuffer(*indexBuffer);

    std::vector<glm::vec3> vertices;
    if (keepOnCpu)
    {
        for (auto i = 0; i < vertexCount; i++)
        {
            vertices.emplace_back(positions[i*3], positions[i*3 + 1], positions[i*3 + 2]);
        }
    }

    auto mesh = Handle<Mesh>::Make(path, vertexBuffer.Pass(), indexBuffer.Pass(), vertexArray.Pass(), vertices);
    mesh->AddSubmesh(Submesh{0, static_cast<int>(triangleCount*3)});

    return mesh;
}
