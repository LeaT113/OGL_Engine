#include "Library/Lighting.glsl"
#include "Library/Lights.glsl"
#include "Library/Tonemapping.glsl"

//#shader vertex
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
uniform mat4 uMVP;
uniform mat4 uModelToWorld;
out vec3 position;
out vec3 normal;

void main()
{
    vec4 positionWS = uModelToWorld * vec4(aPosition, 1.0);
    vec4 normalWS = normalize(uModelToWorld * vec4(aNormal, 0.0));

    position = positionWS.xyz;
    normal = normalWS.xyz;

    gl_Position = uMVP * vec4(aPosition, 1);
}

//#shader fragment
in vec3 position;
in vec3 normal;
layout (location = 0) out vec4 color;

void main()
{
    vec3 normalWS = normalize(normal);

    vec3 lighting = PointLight(position, normalWS, vec3(0, 0.3, 0), 0.1, vec3(0.9, .23, .05) * 0.2) + vec3(0.01, 0.013, 0.02);

    vec3 col = vec3(1,1,1) * lighting;
    col = FilmicToneMapping(col);
    color = vec4(col, 1);
}