#ifndef SCENE_HPP
#define SCENE_HPP

class Entity;

class Scene
{
public:
    void AddEntity(Handle<Entity> entity);

    Entity* GetEntity(const std::string& name) const;

private:
    friend class Serializer;

    std::unordered_map<std::string, Handle<Entity>> _entities;
};



#endif
