#include "Library/Core.glsl"
#include "Library/Tonemapping.glsl"

#pragma Cull(Off)
#pragma DepthWrite(On)
#pragma DepthTest(LEqual)
#pragma Blend(Off)

in vec3 aPosition;

uniform samplerCube SkyboxTex;

struct v2f
{
    vec3 uvw;
};

void vert()
{
    gl_Position = (_ProjectionMatrix * vec4(mat3(_ViewMatrix) * aPosition, 1.0)).xyww;
    v2f.uvw = aPosition;
}

void frag()
{
    vec3 col = texture(SkyboxTex, v2f.uvw).rgb * 0.15f;
    FragOut = vec4(col, 1.0);
}