#include "Entity.hpp"
#include "../Components/TransformComponent.hpp"


size_t Entity::_sIndexCounter = 1;
std::vector<Entity*> Entity::_sEntities;

Entity::Entity(std::string name)
	: _id(_sIndexCounter++)
{
	_name = name.empty() ? "Entity" + std::to_string(_id) : std::move(name);
	_sEntities.push_back(this);
}

size_t Entity::ID() const
{
	return _id;
}

const std::string& Entity::GetName() const
{
	return _name;
}


// Shortcuts
TransformComponent* Entity::GetTransform() const
{
	if (!_transform)
		_transform = GetComponent<TransformComponent>();
	return _transform;
}

Entity* Entity::GetById(size_t id)
{
	if (id-1 >= _sEntities.size())
		return nullptr;
	return _sEntities[id-1];
}
