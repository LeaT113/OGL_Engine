#include "VertexAttributes.hpp"
#include <GL/glew.h>

const VertexAttributeInfo &GetVertexAttributeInfo(VertexAttribute attribute)
{
    static const VertexAttributeInfo attributeInfos[] =
            {
                    {3, GL_FLOAT, GL_FALSE, 3*sizeof(float), "Position"},
                    {3, GL_FLOAT, GL_FALSE, 3*sizeof(float), "Normal"},
                    {3, GL_FLOAT, GL_FALSE, 3*sizeof(float), "Tangent"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord0"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord1"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord2"},
                    {2, GL_FLOAT, GL_FALSE, 2*sizeof(float), "TexCoord3"},
                    {4, GL_FLOAT, GL_FALSE, 4*4*sizeof(float), "InstanceTransform"}
            };

    return attributeInfos[static_cast<int>(attribute)];
}
