#ifndef MATERIALLOADER_HPP
#define MATERIALLOADER_HPP

class Material;

class MaterialLoader
{
public:
    static Handle<Material> LoadMaterial(const std::string& path);

    static void SaveMaterial(const Material& material);

private:
    static const std::string MaterialsPath;
};



#endif
