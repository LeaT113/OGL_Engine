#ifndef OGL_ENGINE_MODELLOADER_HPP
#define OGL_ENGINE_MODELLOADER_HPP

#include "../Scene/Entity.hpp"

class Mesh;

class ModelLoader
{
public:
    static Handle<Mesh> LoadModel(const std::string& path, bool keepOnCpu = false);

private:
    static const std::string ModelsPath;
    static unsigned int PostProcessingFlags;


};


#endif
