#include "Library/Core.glsl"
#include "Library/Lights.glsl"
#include "Library/Tonemapping.glsl"

in vec3 aPosition;
in vec3 aNormal;

uniform vec4 Color;

struct v2f
{
    vec3 position;
    vec3 normal;
    vec2 uv;
};

void vert()
{
    v2f.position = ObjectToWorldPos(aPosition);
    v2f.normal = ObjectToWorldNormal(aNormal);

    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    vec3 normalWS = normalize(v2f.normal);
    vec3 cameraPos = ViewToWorldPos((0).xxx);

    vec3 lighting = ApplyLights(v2f.position, normalWS, cameraPos);

    vec3 col = vec3(1,1,1) * lighting;
    col = FilmicToneMapping(col);
    color = vec4(col, 1);
}
