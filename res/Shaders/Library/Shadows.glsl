#include "Library/Core.glsl"
#include "Library/Lights.glsl"
#include "Library/Scene.glsl"
#define SHADOWS

#define MAX_2D_SHADOWS 2
#define MAX_CUBE_SHADOWS 2

uniform sampler2D _ShadowMaps2D[2];
uniform samplerCube _ShadowMapsCube[2];
uniform mat4 _ShadowMatrices[4];

float PointLight_Shadow(vec3 position, PointLight light)
{
    if (light.shadowIndex == -1)
        return 1;

    vec3 relativePos = (_ShadowMatrices[MAX_2D_SHADOWS + light.shadowIndex] * vec4(position, 1)).xyz;

    float sampleDist = texture(_ShadowMapsCube[light.shadowIndex], relativePos).r * Core.cameraNearFar.y;
    float realDist = dot(relativePos, relativePos);

    return RemapClamped(sampleDist - realDist + 0.05, 0, 0.01, 0, 1);
}

float SpotLight_Shadow(vec3 position, SpotLight light)
{
    if (light.shadowIndex == -1)
        return 1;

    vec4 shadowCoords = _ShadowMatrices[light.shadowIndex] * vec4(position, 1);
    shadowCoords.xyz /= shadowCoords.w;
    shadowCoords = shadowCoords * 0.5 + 0.5;

    float sampleDepth = texture(_ShadowMaps2D[light.shadowIndex], shadowCoords.xy).r;

    return RemapClamped(DepthToLinear(sampleDepth) - DepthToLinear(shadowCoords.z) + 0.01, 0, 0.01, 0, 1);
}
