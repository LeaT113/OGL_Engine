#include "Library/Core.glsl"
#include "Library/Scene.glsl"
#define SHADOWS

uniform sampler2D _ShadowMaps2D[2];
uniform samplerCube _ShadowMapsCube[2];
uniform mat4 _ShadowMatrices[4];

float SpotLight_Shadow(int shadowMapIndex, vec3 worldPos)
{
    if (shadowMapIndex == -1)
        return 1;

    vec4 shadowCoords = _ShadowMatrices[shadowMapIndex] * vec4(worldPos, 1);
    shadowCoords.xyz /= shadowCoords.w;
    shadowCoords = shadowCoords * 0.5 + 0.5;

    float sampleDepth = texture(_ShadowMaps2D[shadowMapIndex], shadowCoords.xy).r;

    return RemapClamped(DepthToLinear(sampleDepth) - DepthToLinear(shadowCoords.z) + 0.01, 0, 0.01, 0, 1);
}

float PointLight_Shadow(int shadowMapIndex, vec3 worldPos)
{
    if (shadowMapIndex == -1)
        return 1;

    vec3 relativePos = (_ShadowMatrices[2 + shadowMapIndex] * vec4(worldPos, 1)).xyz;

    float sampleDist = texture(_ShadowMapsCube[shadowMapIndex], relativePos).r * Core.cameraNearFar.y;
    float realDist = dot(relativePos, relativePos);

    return RemapClamped(sampleDist - realDist + 0.05, 0, 0.01, 0, 1);
}