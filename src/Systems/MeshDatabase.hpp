#ifndef OGL_ENGINE_MESHDATABASE_HPP
#define OGL_ENGINE_MESHDATABASE_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include "../Common/Singleton.hpp"
#include "../Data/MeshData.hpp"

class MeshDatabase : public Singleton<MeshDatabase>
{
public:
	static bool AddMesh(const std::string& name, uint32_t vertexCount, const float *vertices, const float *normals,
						uint32_t triangleCount, const unsigned int *indices);

	static bool LoadMesh(const std::string &fileName);

	static const MeshData *GetMesh(const std::string &name);

private:
	static constexpr std::string ResourcesPath = "res/";

	std::unordered_map<std::string, std::unique_ptr<MeshData>> _meshes;
};


#endif
