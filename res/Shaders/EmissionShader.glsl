#include "Library/Core.glsl"
#include "Library/Tonemapping.glsl"

in vec3 aPosition;

uniform vec4 Color;

struct v2f
{
    vec3 position;
    vec2 uv;
};

void vert()
{
    v2f.position = ObjectToWorldPos(aPosition);

    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    vec3 col = Color.xyz;
    color = vec4(col, 1);
}