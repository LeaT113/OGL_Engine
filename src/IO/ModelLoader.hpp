#ifndef OGL_ENGINE_MODELLOADER_HPP
#define OGL_ENGINE_MODELLOADER_HPP

#include <string>
#include "../Scene/Entity.hpp"

class Mesh;

class ModelLoader
{
public:
    static Handle<Mesh> LoadModel(const std::string& path);

private:
    static const std::string ModelsPath;
    static unsigned int PostProcessingFlags;


};


#endif
