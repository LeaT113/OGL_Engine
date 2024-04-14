#include "Library/Core.glsl"

#pragma Blend(Alpha)

in vec3 aPosition;

struct v2f
{
    vec3 color;
};

void vert()
{
    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    color = vec4(0, 0.3, 0.5, 0.3);
}