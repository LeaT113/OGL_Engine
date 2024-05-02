#include "Library/Core.glsl"

in vec3 aPosition;

struct v2f
{
    vec3 viewPos;
};

void vert()
{
    gl_Position = ObjectToClipPos(aPosition);

    v2f.viewPos = (_ViewMatrix * _ModelMatrix * vec4(aPosition, 1)).xyz;
}

void frag()
{
    gl_FragDepth = dot(v2f.viewPos, v2f.viewPos) / Core.cameraNearFar.y;
}