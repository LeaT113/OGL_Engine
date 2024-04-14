#ifndef MESHFACTORY_HPP
#define MESHFACTORY_HPP

#include "../Core/Handle.hpp"
#include "../Resources/Mesh.hpp"

class MeshFactory
{
public:
    static Handle<Mesh> CreateQuad();
};



#endif
