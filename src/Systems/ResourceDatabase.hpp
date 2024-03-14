#ifndef OGL_ENGINE_RESOURCEDATABASE_HPP
#define OGL_ENGINE_RESOURCEDATABASE_HPP

#include <string>
#include <unordered_map>
#include "../Core/Singleton.hpp"
#include "../Resources/Mesh.hpp"
#include "../Resources/Shader.hpp"


class Material;

class ResourceDatabase : public Singleton<ResourceDatabase>
{
public:
    static bool AddMesh(Handle<Mesh> mesh);
    static const Mesh* GetMesh(const std::string& name);

	static bool AddShader(Handle<Shader> shader);
	static const Shader* GetShader(const std::string &name);
	static void ReloadShaders();

	static bool AddTexture(Handle<Texture> texture);
	static Texture* GetTexture(const std::string& name);

	static bool AddMaterial(Handle<Material> material);
	static const Material* GetMaterial(const std::string &name);

private:
	static const std::string ShadersPath;

    std::unordered_map<std::string, Handle<Mesh>> _meshLibrary;
    std::unordered_map<std::string, Handle<Shader>> _shaderLibrary;
	std::unordered_map<std::string, Handle<Texture>> _textureLibrary;
	std::unordered_map<std::string, Handle<Material>> _materialLibrary;
};


#endif
