#include "VertexAttributes.hpp"

const VertexAttributeInfo &GetVertexAttributeInfo(VertexAttribute attribute)
{
    static constexpr VertexAttributeInfo attributeInfos[] =
            {
                    {3, GL_FLOAT, GL_FALSE, 3*sizeof(float), "Position"},
                    {3, GL_FLOAT, GL_FALSE, 3*sizeof(float), "Normal"},
                    {3, GL_FLOAT, GL_FALSE, 3*sizeof(float), "Tangent"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord0"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord1"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord2"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord3"},
            };

    if((int)attribute >= (int)VertexAttribute::COUNT)
        throw std::runtime_error("Shader::GetAttributeInfo out of range");

    return attributeInfos[attribute];
}
