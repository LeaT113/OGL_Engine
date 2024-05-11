#ifndef OGL_ENGINE_VERTEXATTRIBUTES_HPP
#define OGL_ENGINE_VERTEXATTRIBUTES_HPP

#include <stdexcept>


enum class VertexAttribute
{
    Position = 0,
    Normal = 1,
    Tangent = 2,
    TexCoord0 = 3,
    TexCoord1 = 4,
    TexCoord2 = 5,
    TexCoord3 = 6,
    InstanceTransform = 7,
    LAST = 7,
};

struct VertexAttributeInfo
{
    int size;
    unsigned int type;
    unsigned int normalized;
    int byteSize;
    std::string name;
};

const VertexAttributeInfo &GetVertexAttributeInfo(VertexAttribute attribute);

#endif
