#include <stdexcept>
#include <GL/glew.h>
#include "Shader.hpp"


Shader::AttributeInfo Shader::_attributeInfos[] =
{
        {3, GL_FLOAT, GL_FALSE, 3*sizeof(float)},   // Position
        {3, GL_FLOAT, GL_FALSE, 3*sizeof(float)},   // Normal
        {3, GL_FLOAT, GL_FALSE, 3*sizeof(float)},   // Tangent
        {2, GL_FLOAT, GL_FALSE, 2*sizeof(float)},   // TexCoord0
        {2, GL_FLOAT, GL_FALSE, 2*sizeof(float)},   // TexCoord1
        {2, GL_FLOAT, GL_FALSE, 2*sizeof(float)},   // TexCoord2
        {2, GL_FLOAT, GL_FALSE, 2*sizeof(float)},   // TexCoord3
};

const Shader::AttributeInfo &Shader::GetAttributeInfo(Shader::Attribute attribute)
{
    if((int)attribute >= (int)Shader::Attribute::COUNT)
        throw std::runtime_error("Shader::GetAttributeInfo out of range");

    return _attributeInfos[attribute];
}
