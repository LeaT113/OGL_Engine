#ifndef OGL_ENGINE_RESOURCEDATABASE_HPP
#define OGL_ENGINE_RESOURCEDATABASE_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include "../Common/Singleton.hpp"
#include "../Data/MeshData.hpp"
#include "../Data/ShaderData.hpp"


class ResourceDatabase : public Singleton<ResourceDatabase>
{
public:
	static bool AddMesh(const std::string &name, uint32_t vertexCount, const float *vertices, const float *normals,
						uint32_t triangleCount, const unsigned int *indices);

	static bool LoadMesh(const std::string &fileName);

	static const MeshData *GetMesh(const std::string &name);


	static bool AddShader(const std::string &name, const std::set<std::string> &uniforms, const std::string &vertexShader, const std::string &fragmentShader);

	static bool LoadShader(const std::string &fileName);

	static const ShaderData *GetShader(const std::string &name);

private:
	static const std::string ModelsPath;
	static const std::string ShadersPath;

	std::unordered_map<std::string, std::unique_ptr<MeshData>> _meshes;
	std::unordered_map<std::string, std::unique_ptr<ShaderData>> _shaders;
};


#endif
