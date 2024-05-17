#ifndef RESOURCE_HPP
#define RESOURCE_HPP

using SerializedResource = std::unordered_map<std::string, std::string>;

class Resource
{
public:
    Resource(std::string name);
    virtual ~Resource() = default;

    std::string Name() const;

protected:
    std::string _name;
};

#endif
