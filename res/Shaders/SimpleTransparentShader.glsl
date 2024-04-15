#include "Library/Core.glsl"

#pragma Blend(Alpha)
#pragma DepthWrite(Off)
#pragma Cull(Off)

in vec3 aPosition;

uniform vec3 Color;

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
    color = vec4(Color, 0.3);
}