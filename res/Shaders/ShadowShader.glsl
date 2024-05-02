#include "Library/Core.glsl"

in vec3 aPosition;

void vert()
{
    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    color = vec4(1);
}