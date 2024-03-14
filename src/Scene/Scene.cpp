#include "Scene.hpp"

#include "Entity.hpp"

void Scene::AddEntity(Handle<Entity> entity)
{
    std::string name = entity->GetName();
    _entities.emplace(name, entity.Pass());
}

Entity* Scene::GetEntity(const std::string& name) const
{
    auto it = _entities.find(name);
    if (it == _entities.end())
        return nullptr;

    return it->second.Access();
}
