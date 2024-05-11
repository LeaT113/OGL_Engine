#include "Library/Core.glsl"

in vec3 aPosition;
in mat4 aInstanceTransform;

struct v2f
{
    vec3 viewPos;
};

void vert()
{
    if (_Instanced)
    {
        vec4 viewPos = _ViewMatrix * aInstanceTransform * vec4(aPosition, 1);
        gl_Position = _ProjectionMatrix * viewPos;
        v2f.viewPos = viewPos.xyz;
    }
    else
    {
        v2f.viewPos = (_ViewMatrix * _ModelMatrix * vec4(aPosition, 1)).xyz;
        gl_Position = ObjectToClipPos(aPosition);
    }
}

void frag()
{
    gl_FragDepth = dot(v2f.viewPos, v2f.viewPos) / Core.cameraNearFar.y;
}