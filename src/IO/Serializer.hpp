#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <yaml-cpp/yaml.h>

class Scene;
class CameraComponent;
class LightComponent;
class RendererComponent;
class TransformComponent;
class Component;
class Entity;

class Serializer
{
public:
    static Handle<Scene> LoadScene(std::string path);

    static void SaveScene(const Scene& scene, std::string path);

private:
    static YAML::Node SerializeEntity(const Entity& entity);

    static Handle<Entity> DeserializeEntity(const YAML::Node& entityYaml);

    template<typename T>
    static YAML::Node SerializeComponent(const T& component)
    {
        throw std::runtime_error("Serializer::SerializeComponent Unsupported component");
    }

    template<typename T>
    static Handle<T> DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner)
    {
        throw std::runtime_error("Serializer::DeserializeComponent Unsupported component");
    }
};

template<>
YAML::Node Serializer::SerializeComponent<CameraComponent>(const CameraComponent& component);

template<>
YAML::Node Serializer::SerializeComponent<LightComponent>(const LightComponent& component);

template<>
YAML::Node Serializer::SerializeComponent<RendererComponent>(const RendererComponent& component);

template<>
YAML::Node Serializer::SerializeComponent<TransformComponent>(const TransformComponent& component);

template<>
Handle<LightComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner);

template<>
Handle<CameraComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner);

template<>
Handle<RendererComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner);

template<>
Handle<TransformComponent> Serializer::DeserializeComponent(const YAML::Node& componentYaml, const Entity& owner);

#endif
