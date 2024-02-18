#include "Library/Core.glsl"
#include "Library/Lights.glsl"
#include "Library/Tonemapping.glsl"

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord0;

uniform vec4 Color;
uniform sampler2D AlbedoTex;

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
    v2f.uv = aTexCoord0;

    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    vec3 normalWS = normalize(v2f.normal);
    vec3 cameraPos = ViewToWorldPos((0).xxx);

    vec3 lighting = ApplyLights(v2f.position, normalWS, cameraPos);

    vec4 tex = texture(AlbedoTex, v2f.uv);
    vec3 col = pow(tex.rgb, vec3(2.2)) * lighting;
    col = FilmicToneMapping(col);
    color = vec4(col, 1);
}
