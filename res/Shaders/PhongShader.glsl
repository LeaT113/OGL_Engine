#include "Library/Core.glsl"
#include "Library/Lights.glsl"
#include "Library/Tonemapping.glsl"

in vec3 aPosition;
in vec3 aNormal;

uniform vec4 Color;
uniform sampler2D Albedo;
uniform sampler2D Other;

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
    vec2 uv = v2f.position.xz + v2f.position.y;
    vec4 tex = texture(Albedo, uv);
    vec4 tex2 = texture(Other, uv * 0.5);
    col = FilmicToneMapping(col * tex.xyz * (1 - tex2.xyz));
    color = vec4(col, 1);
}
