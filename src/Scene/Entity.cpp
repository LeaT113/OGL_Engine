#include "Entity.hpp"
#include "../Components/TransformComponent.hpp"


size_t Entity::_sIndexCounter;

Entity::Entity(std::string name)
	: _id(_sIndexCounter++)
{
	_name = name.empty() ? "Entity" + std::to_string(_id) : std::move(name);
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
TransformComponent* Entity::Transform()
{
	return dynamic_cast<TransformComponent *>(GetComponent<TransformComponent>());
}