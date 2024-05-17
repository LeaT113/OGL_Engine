#include "ResourceDatabase.hpp"
#include "../Resources/Material.hpp"
#include "../IO/ShaderLoader.hpp"


const std::string ResourceDatabase::ShadersPath = "res/Shaders/";


bool ResourceDatabase::AddMesh(Handle<Mesh> mesh)
{
	auto name = mesh->Name();
	auto existing = GetMesh(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase::AddMesh Mesh with name '" + mesh->Name() + "' already exists");

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

bool ResourceDatabase::AddShader(Handle<Shader> shader)
{
	auto name = shader->Name();
	auto existing = GetShader(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase::AddShader Shader with name '" + shader->Name() + "' already exists");

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
        shader->Replace(newShader.Release()); // TODO
    }
}

bool ResourceDatabase::AddTexture(Handle<Texture> texture)
{
	auto name = texture->Name();
	auto existing = GetTexture(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase::AddTexture Texture with name '" + texture->Name() + "' already exists");

	Instance()._textureLibrary.emplace(name, texture.Pass());

	return true;
}

Texture* ResourceDatabase::GetTexture(const std::string& name)
{
	auto it = Instance()._textureLibrary.find(name);
	if (it == Instance()._textureLibrary.end())
		return nullptr;

	return it->second.Access();
}

bool ResourceDatabase::AddMaterial(Handle<Material> material)
{
	auto name = material->Name();
	auto existing = GetMaterial(name);
	if (existing != nullptr)
		throw std::runtime_error("ResourceDatabase::AddMaterial Material with name '" + material->Name() + "' already exists");

	Instance()._materialLibrary.emplace(name, material.Pass());

	return true;
}

const Material* ResourceDatabase::GetMaterial(const std::string& name)
{
	auto it = Instance()._materialLibrary.find(name);
	if (it == Instance()._materialLibrary.end())
		return nullptr;

	return it->second.Access();
}
