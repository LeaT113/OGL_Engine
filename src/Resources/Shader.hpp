#ifndef OGL_ENGINE_SHADER_HPP
#define OGL_ENGINE_SHADER_HPP


class Shader
{
public:
    enum Attribute
    {
        Position = 0,
        Normal = 1,
        Tangent = 2,
        TexCoord0 = 3,
        TexCoord1 = 4,
        TexCoord2 = 5,
        TexCoord3 = 6,
        COUNT = 7
    };

    struct AttributeInfo
    {
        int size;
        unsigned int type;
        unsigned int normalized;
        int byteSize;
    };

    const static AttributeInfo &GetAttributeInfo(Attribute attribute);

private:
    static AttributeInfo _attributeInfos[];
};


#endif //OGL_ENGINE_SHADER_HPP
