#include "Component.hpp"

Component::Component(const Entity& owner) : _owner(&owner)
{}

Component::~Component() = default;
