#include "ResourceDatabase.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include "../Resources/ShaderLoader.hpp"


const std::string ResourceDatabase::ShadersPath = "res/Shaders/";


bool ResourceDatabase::AddMesh(const std::string &name, Handle<Mesh> mesh)
{
    auto existing = GetMesh(name);
    if (existing != nullptr)
        throw std::runtime_error("ResourceDatabase mesh with name '" + name + "' already exists");

    Instance()._meshLibrary.emplace(name, mesh.Pass());

    return true;
}

const Mesh *ResourceDatabase::GetMesh(const std::string &name)
{
    auto &instance = Instance();
    auto it = instance._meshLibrary.find(name);
    if (it == instance._meshLibrary.end())
        return nullptr;

    return it->second.Access();
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

bool ResourceDatabase::AddShader(const std::string &name, Handle<Shader> shader)
{
	auto existing = GetShader(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase shader with name '" + name + "' already exists");

	Instance()._shaderLibrary.emplace(name, shader.Pass());

	return true;
}

const Shader *ResourceDatabase::GetShader(const std::string &name)
{
	auto &instance = Instance();
	auto it = instance._shaderLibrary.find(name);
	if (it == instance._shaderLibrary.end())
		return nullptr;

	return it->second.Access();
}

void ResourceDatabase::ReloadShaders()
{
    for(const auto& [name, shader] : Instance()._shaderLibrary)
    {
        auto newShader = ShaderLoader::LoadShader("");
        shader->Replace(newShader.Release());
    }
}
