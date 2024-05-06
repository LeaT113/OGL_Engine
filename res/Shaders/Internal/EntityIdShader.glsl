#include "Library/Core.glsl"

in vec3 aPosition;

uniform uint Id;

out uvec4 FragOut;

void vert()
{
    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    FragOut = uvec4(Id, 0, 0, 0);
}