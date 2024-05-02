#include "Serializer.hpp"

#include <fstream>
#include <glm/gtc/quaternion.hpp>

#include "../Scene/Entity.hpp"
#include "../Components/CameraComponent.hpp"
#include "../Components/LightComponent.hpp"
#include "../Components/RendererComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "YAMLExtensions.hpp"
#include "../Scene/Scene.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/ResourceDatabase.hpp"

Handle<Scene> Serializer::LoadScene(std::string path)
{
    auto sceneYaml = YAML::LoadFile("res/Scenes/" + path);

    auto scene = Handle<Scene>::Make();

    if (!sceneYaml.IsSequence())
        throw std::runtime_error("Serializer::LoadScene Invalid scene file");
    for(auto entityYaml : sceneYaml)
    {
        scene->AddEntity(DeserializeEntity(entityYaml));
    }

    return scene;
}

void Serializer::SaveScene(const Scene& scene, std::string path)
{
    auto entitiesYaml = YAML::Node(YAML::NodeType::Sequence);

    for (const auto& [key, entity] : scene._entities)
    {
        entitiesYaml.push_back(SerializeEntity(*entity));
    }

    std::ofstream outFile("res/Scenes/" + path);
    outFile << entitiesYaml << std::endl;
    outFile.close();
}

Handle<Entity> Serializer::DeserializeEntity(const YAML::Node& entityYaml)
{
    auto entity = Handle<Entity>::Make(entityYaml["Entity"]["_name"].as<std::string>());
    auto componentsYaml = entityYaml["Entity"]["_components"];

    // Transform has priority
    if (!componentsYaml["TransformComponent"])
        throw std::runtime_error("Serializer::DeserializeEntity Entity has no TransformComponent");
    entity->_components.emplace(typeid(TransformComponent), DeserializeComponent<TransformComponent>(componentsYaml["TransformComponent"], *entity));

    // Load other components
    for (const auto& componentYaml : componentsYaml)
    {
        auto componentName = componentYaml.first.as<std::string>();
        auto comp = componentYaml.second;

        if (componentName == "CameraComponent")
            entity->_components.emplace(typeid(CameraComponent), DeserializeComponent<CameraComponent>(comp, *entity));
        else if (componentName == "LightComponent")
            entity->_components.emplace(typeid(LightComponent), DeserializeComponent<LightComponent>(comp, *entity));
        else if (componentName == "RendererComponent")
            entity->_components.emplace(typeid(RendererComponent), DeserializeComponent<RendererComponent>(comp, *entity));
    }

    return entity;
}

YAML::Node Serializer::SerializeEntity(const Entity& entity)
{
    YAML::Node outYaml;
    auto entityYaml = outYaml["Entity"];
    entityYaml["_name"] = entity._name;
    auto components = entityYaml["_components"];

    for (const auto& [type, component] : entity._components)
    {
        if(type == typeid(CameraComponent))
            components["CameraComponent"] = SerializeComponent(*dynamic_cast<CameraComponent*>(component.Access()));
        else if (type == typeid(LightComponent))
            components["LightComponent"] = SerializeComponent(*dynamic_cast<LightComponent*>(component.Access()));
        else if (type == typeid(RendererComponent))
            components["RendererComponent"] = SerializeComponent(*dynamic_cast<RendererComponent*>(component.Access()));
        else if (type == typeid(TransformComponent))
            components["TransformComponent"] = SerializeComponent(*dynamic_cast<TransformComponent*>(component.Access()));
        else
            throw std::runtime_error("Serializer::SerializeEntity Unsupported component " + std::string(type.name()));
    }

    return outYaml;
}

template<>
YAML::Node Serializer::SerializeComponent<CameraComponent>(const CameraComponent& component)
{
    auto properties = YAML::Node(YAML::NodeType::Map);

    properties["_projectionType"] = static_cast<int>(component._projectionType);
    properties["_horizontalFov"] = component._horizontalFov;

    return properties;
}

template<>
YAML::Node Serializer::SerializeComponent<LightComponent>(const LightComponent& component)
{
    auto properties = YAML::Node(YAML::NodeType::Map);

    properties["_lightType"] = static_cast<int>(component._lightType);
    properties["_color"] = component._color;
    properties["_direction"] = component._direction;
    properties["_spotInnerAngle"] = component._spotInnerAngle;
    properties["_spotOuterAngle"] = component._spotOuterAngle;
    properties["_shadowCasting"] = component._shadowCasting;

    return properties;
}

template<>
YAML::Node Serializer::SerializeComponent<RendererComponent>(const RendererComponent& component)
{
    auto properties = YAML::Node(YAML::NodeType::Map);

    properties["_mesh"] = component._mesh->Name();
    properties["_material"] = component._material->Name();

    return properties;
}

template<>
YAML::Node Serializer::SerializeComponent<TransformComponent>(const TransformComponent& component)
{
    auto properties = YAML::Node(YAML::NodeType::Map);

    properties["_position"] = component._position;
    properties["_rotation"] = component._rotation;
    properties["_scale"] = component._scale;

    return properties;
}

template<>
Handle<CameraComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner)
{
    auto component = Handle<CameraComponent>::Make(owner);

    component->_projectionType = static_cast<CameraComponent::ProjectionType>(componentYaml["_projectionType"].as<int>());
    component->_horizontalFov = componentYaml["_horizontalFov"].as<float>();

    return component;
}

template<>
Handle<LightComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner)
{
    auto component = Handle<LightComponent>::Make(owner, static_cast<LightComponent::Type>(componentYaml["_lightType"].as<int>()));

    component->_color = componentYaml["_color"].as<glm::vec3>();
    component->_direction = componentYaml["_direction"].as<glm::vec3>();
    component->_spotInnerAngle = componentYaml["_spotInnerAngle"].as<float>();
    component->_spotOuterAngle = componentYaml["_spotOuterAngle"].as<float>();
    component->_shadowCasting = componentYaml["_shadowCasting"].as<bool>();

    return component;
}

template<>
Handle<RendererComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner)
{
    auto component = Handle<RendererComponent>::Make(owner);

    component->_mesh = ResourceDatabase::GetMesh(componentYaml["_mesh"].as<std::string>());
    component->_material = ResourceDatabase::GetMaterial(componentYaml["_material"].as<std::string>());

    RenderSystem::RegisterRenderer(component.Access());

    return component;
}

template<>
Handle<TransformComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner)
{
    auto component = Handle<TransformComponent>::Make(owner);

    component->_position = componentYaml["_position"].as<glm::vec3>();
    component->_rotation = componentYaml["_rotation"].as<glm::quat>();
    component->_scale = componentYaml["_scale"].as<glm::vec3>();

    return component;
}
