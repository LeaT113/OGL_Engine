#include "MeshDatabase.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>


bool MeshDatabase::AddMesh(const std::string &name, uint32_t vertexCount, const float *vertices, const float *normals,
						   uint32_t triangleCount, const unsigned int *indices)
{
	auto &instance = Instance();
	auto it = instance._meshes.find(name);
	if (it != instance._meshes.end())
		throw std::runtime_error("MeshDatabase mesh with name '" + name + "' already exists");

	instance._meshes.emplace(name, std::make_unique<MeshData>(vertexCount, vertices, normals, triangleCount, indices));

	return true;
}

bool MeshDatabase::LoadMesh(const std::string &fileName)
{
	std::string filePath = ResourcesPath + "Models/" + fileName;

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate    // Triangulate polygons
													   |
													   aiProcess_PreTransformVertices    // Transform scene hierarchy into one root with geometry-leafs only
													   |
													   aiProcess_GenSmoothNormals        // Calculate normals per vertex
													   //| aiProcess_CalcTangentSpace      	// Calculate tangents per vertex
													   | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr)
		throw std::runtime_error("assimp failed to load scene");
	if (scene->mNumMeshes != 1)
		throw std::runtime_error("assimp multiple meshes in file");

	const aiMesh *mesh = scene->mMeshes[0];

	// Vertices
	uint32_t vertexCount = mesh->mNumVertices;
	auto vertices = reinterpret_cast<float *>(mesh->mVertices);

	// Normals
	if (!mesh->HasNormals())
		throw std::runtime_error("assimp mesh has no normals");
	auto *normals = reinterpret_cast<float *>(mesh->mNormals);

	// TODO UVs

	// Indices
	uint32_t triangleCount = mesh->mNumFaces;
	std::vector<unsigned int> indices(triangleCount * 3);
	for (size_t i = 0; i < triangleCount; i++)
	{
		indices[i + 0] = mesh->mFaces[i].mIndices[0];
		indices[i + 1] = mesh->mFaces[i].mIndices[1];
		indices[i + 2] = mesh->mFaces[i].mIndices[2];
	}

	AddMesh(fileName, vertexCount, vertices, normals, triangleCount, indices.data());

	return true;
}

const MeshData *MeshDatabase::GetMesh(const std::string &name)
{
	auto &instance = Instance();
	auto it = instance._meshes.find(name);
	if (it == instance._meshes.end())
		return nullptr;

	return it->second.get();
}
