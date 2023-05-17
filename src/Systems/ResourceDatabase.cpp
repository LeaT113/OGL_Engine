#include "ResourceDatabase.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>


const std::string ResourceDatabase::ModelsPath = "res/Models/";
const std::string ResourceDatabase::ShadersPath = "res/Shaders/";


bool
ResourceDatabase::AddMesh(const std::string &name, uint32_t vertexCount, const float *vertices, const float *normals,
						  uint32_t triangleCount, const unsigned int *indices)
{
	auto existing = GetMesh(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase mesh with name '" + name + "' already exists");

	Instance()._meshes.emplace(name,
							   std::make_unique<MeshData>(vertexCount, vertices, normals, triangleCount, indices));

	return true;
}

bool ResourceDatabase::LoadMesh(const std::string &fileName)
{
	std::string filePath = ModelsPath + fileName;

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

const MeshData *ResourceDatabase::GetMesh(const std::string &name)
{
	auto &instance = Instance();
	auto it = instance._meshes.find(name);
	if (it == instance._meshes.end())
		return nullptr;

	return it->second.get();
}


bool
ResourceDatabase::AddShader(const std::string &name, const std::set<std::string> &uniforms, const std::string &vertexShader, const std::string &fragmentShader)
{
	auto existing = GetShader(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase shader with name '" + name + "' already exists");

	Instance()._shaders.emplace(name,
								std::make_unique<ShaderData>(uniforms, vertexShader, fragmentShader));

	return true;
}

bool ResourceDatabase::LoadShader(const std::string &fileName)
{
	static const std::string version = "#version 450 core";

	std::ifstream stream(ShadersPath + fileName);

	std::stringstream includes;
	std::set<std::string> uniforms;
	std::stringstream vertexShader;
	std::stringstream fragmentShader;

	uint8_t mode = 0;    // 0 - none, 1 - vertex, 2 - fragment
	std::string line;
	while (getline(stream, line))
	{
		if (line.length() == 0)
			continue;

		// Include
		auto inclFind = line.find("#include");
		if (inclFind != std::string::npos)
		{
			if (mode != 0)
				throw std::runtime_error("ResourceDatabase shader include after shader type declaration");

			auto path = ShadersPath + line.substr(inclFind + 10, line.length() - inclFind - 11);
			std::ifstream includedFile(path);
			includes << includedFile.rdbuf();
			continue;
		}

		// Shader type
		if (line.starts_with("//#shader"))
		{
			if (line.find("vertex") != std::string::npos && mode == 0)
			{
				vertexShader << version << '\n' << includes.str() << '\n';
				mode = 1;
			} else if (line.find("fragment") != std::string::npos && mode == 1)
			{
				fragmentShader << version << '\n' << includes.str() << '\n';
				mode = 2;
			} else
				throw std::runtime_error("ResourceDatabase wrong shader type specification");

			continue;
		}

		// Code line
		auto findUniform = line.find("uniform ");
		if(findUniform != std::string::npos) // Uniform collection
		{
			// TODO Do in ShaderData
			uniforms.emplace(line.substr(findUniform+8, line.length()-findUniform-9));
		}
		switch (mode) // Append
		{
			case 1:
				vertexShader << line << '\n';
				break;

			case 2:
				fragmentShader << line << '\n';
				break;

			default:
				throw std::runtime_error("ResourceDatabase illegal shader code layout");
		}
	}

	AddShader(fileName, uniforms, vertexShader.str(), fragmentShader.str());

	return true;
}

const ShaderData *ResourceDatabase::GetShader(const std::string &name)
{
	auto &instance = Instance();
	auto it = instance._shaders.find(name);
	if (it == instance._shaders.end())
		return nullptr;

	return it->second.get();
}
