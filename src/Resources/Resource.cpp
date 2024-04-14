#include "Resource.hpp"

Resource::Resource(std::string name)
    : _name(std::move(name))
{}

std::string Resource::Name() const
{
    return _name;
}
