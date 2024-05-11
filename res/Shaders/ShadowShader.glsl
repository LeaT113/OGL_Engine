#include "Library/Core.glsl"

in vec3 aPosition;
in mat4 aInstanceTransform;


void vert()
{
    if (_Instanced)
        gl_Position = _ProjectionMatrix * _ViewMatrix * aInstanceTransform * vec4(aPosition, 1);
    else
        gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    FragOut = vec4(1);
}