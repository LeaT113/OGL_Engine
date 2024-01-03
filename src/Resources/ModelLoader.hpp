#ifndef OGL_ENGINE_MODELLOADER_HPP
#define OGL_ENGINE_MODELLOADER_HPP

#include <string>
#include "../Entity/Entity.hpp"


class ModelLoader
{
public:
    /// Individually load all meshes contained in a file into memory
    /// \param path Relative path inside Models folder
    //void LoadMeshes(std::string path);

    /// Load and construct
    /// \param path Relative path inside Models folder
    /// \return Render-able entity
    static Entity* LoadModel(const std::string& path);

private:
    static const std::string ModelsPath;
    static unsigned int PostProcessingFlags;


};


#endif
