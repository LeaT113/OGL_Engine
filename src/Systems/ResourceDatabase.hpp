#ifndef OGL_ENGINE_RESOURCEDATABASE_HPP
#define OGL_ENGINE_RESOURCEDATABASE_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include "../Core/Singleton.hpp"
#include "../Data/ShaderData.hpp"
#include "../Resources/Mesh.hpp"
#include "../Resources/Shader.hpp"


class ResourceDatabase : public Singleton<ResourceDatabase>
{
public:
    static bool AddMesh(const std::string &name, Handle<Mesh> mesh);
    static const Mesh* GetMesh(const std::string& name);

	static bool AddShader(const std::string &name, Handle<Shader> shader);
	static const Shader* GetShader(const std::string &name);
	static void ReloadShaders();

	static bool AddShader(const std::string &name, const std::set<std::string> &uniforms, const std::string &vertexShader, const std::string &fragmentShader);
	static bool LoadShader(const std::string &fileName);

	//static const ShaderData *GetShader(const std::string &name);

private:
	static const std::string ShadersPath;

    std::unordered_map<std::string, Handle<Mesh>> _meshLibrary;
    std::unordered_map<std::string, Handle<Shader>> _shaderLibrary;
	std::unordered_map<std::string, std::unique_ptr<ShaderData>> _shaders;
};


#endif
