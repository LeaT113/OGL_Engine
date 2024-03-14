#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include <string>
#include <unordered_map>

using SerializedResource = std::unordered_map<std::string, std::string>;

class Resource
{
public:
    Resource() = default;
    virtual ~Resource() = default;

    std::string Name() const;

protected:
    std::string _name;
};

#endif
