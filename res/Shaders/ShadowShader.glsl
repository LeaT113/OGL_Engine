#include "Library/Core.glsl"

in vec3 aPosition;

void vert()
{
    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    FragOut = vec4(1);
}