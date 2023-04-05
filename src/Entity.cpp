#include "Entity.hpp"
#include "Components/TransformComponent.hpp"


size_t Entity::_sIndexCounter;

Entity::Entity()
{
	_id = _sIndexCounter++;
}

size_t Entity::ID() const
{
	return _id;
}


// Shortcuts
TransformComponent* Entity::Transform()
{
	return dynamic_cast<TransformComponent *>(GetComponent<TransformComponent>());
}