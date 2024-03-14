#include "MaterialLoader.hpp"
#include <fstream>
#include <unordered_map>
#include "../Resources/Material.hpp"
#include <yaml-cpp/yaml.h>
#include "YAMLExtensions.hpp"
#include "../Systems/ResourceDatabase.hpp"

const std::string MaterialLoader::MaterialsPath = "res/Materials/";

Handle<Material> MaterialLoader::LoadMaterial(const std::string& path)
{
    YAML::Node matYaml = YAML::LoadFile(MaterialsPath + path);

    auto name = matYaml["_name"].as<std::string>();

    auto shaderName = matYaml["_shader"].as<std::string>();
    auto shader = ResourceDatabase::GetShader(shaderName);
    if (!shader)
        throw std::runtime_error("MaterialLoader::LoadMaterial Shader does not exist");

    if(!matYaml["_uniforms"].IsMap() || !matYaml["_textures"].IsMap())
        throw std::runtime_error("MaterialLoader::LoadMaterial Invalid material");

    auto material = Handle<Material>::Make(*shader, path);

    // Apply uniforms
    for (auto uniform : matYaml["_uniforms"])
    {
        auto key = uniform.first.as<std::string>();
        auto typeIt = shader->GetUniforms().find(key);
        if(typeIt == shader->GetUniforms().end())
            throw std::runtime_error("MaterialLoader::LoadMaterial Corresponding uniform not found");
        auto type = typeIt->second.type;

        if (type == typeid(bool))
            material->Set(key, uniform.second.as<bool>());
        else if (type == typeid(float))
            material->Set(key, uniform.second.as<float>());
        else if (type == typeid(glm::vec2))
            material->Set(key, uniform.second.as<glm::vec2>());
        else if (type == typeid(glm::vec3))
            material->Set(key, uniform.second.as<glm::vec3>());
        else if (type == typeid(glm::vec4))
            material->Set(key, uniform.second.as<glm::vec4>());
        else if (type == typeid(glm::mat3))
            material->Set(key, uniform.second.as<glm::mat3>());
        else if (type == typeid(glm::mat4))
            material->Set(key, uniform.second.as<glm::mat4>());
        else
            throw std::runtime_error("MaterialLoader::LoadMaterial Unsupported deserialization type " + std::string(type.name()));
    }

    // Apply textures
    for (auto texture : matYaml["_textures"])
    {
        auto key = texture.first.as<std::string>();
        if (texture.second.IsNull())
            continue;
        auto tex = texture.second.as<std::string>();

        auto texRes = ResourceDatabase::GetTexture(tex);
        if(texRes == nullptr)
            throw std::runtime_error("MaterialLoader::LoadMaterial Texture '" + tex + "' not found");

        material->Set(key, texRes);
    }

    return material;
}

void MaterialLoader::SaveMaterial(const Material& material)
{
    YAML::Node node;
    node["_name"] = material._name;
    node["_shader"] = material._shader.Name();
    node["_uniforms"] = YAML::Node(YAML::NodeType::Map);
    node["_textures"] = YAML::Node(YAML::NodeType::Map);

    // Uniforms
    for (const auto& uniform : material._uniformValues)
    {
        // Find name in shader
        std::string name;
        for (const auto & uniformSlot : material._shader.GetUniforms())
            if (uniformSlot.second.location == uniform.first)
            {
                name = uniformSlot.first;
                break;
            }
        if (name.empty())
            throw std::runtime_error("MaterialLoader::SaveMaterial No corresponding uniform in shader");

        // Write value
        std::visit([node, name](auto&& value) mutable
        {
            node["_uniforms"][name] = value;
        }, uniform.second);
    }

    // Textures
    for (const auto& texture : material._textureValues)
    {
        std::string name;
        for (const auto & textureSlot : material._shader.GetTextureSlots())
            if (textureSlot.second.unit == texture.first)
            {
                name = textureSlot.first;
                break;
            }

        if(texture.second == nullptr)
            node["_textures"][name] = YAML::Null;
        else
            node["_textures"][name] = texture.second->Name();
    }

    std::ofstream out(MaterialsPath + material._name);
    out << node << std::endl;
    out.close();
}
