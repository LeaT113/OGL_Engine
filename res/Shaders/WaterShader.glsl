#include "Library/Core.glsl"
#include "Library/Scene.glsl"
#include "Library/Noise.glsl"
#include "Library/Optics.glsl"

#pragma Blend(Alpha)
#pragma DepthWrite(Off)
#pragma Cull(Back)

#define MAX_DEPTH_STEPS 80
#define DEPTH_THICKNESS 0.1

#define IOR 1.33

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTexCoord0;

uniform float WaveScale = 12;
uniform float WaveStrength = 0.15;
uniform float WaveSpeed = 6;
uniform vec3 AbsorptionsColor = vec3(0.6, 0.2, 0.3);

struct v2f
{
    vec2 uv;
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
};

void vert()
{
    v2f.uv = aTexCoord0;
    v2f.position = ObjectToWorldPos(aPosition);
    v2f.normal = ObjectToWorldNormal(aNormal);
    v2f.tangent = ObjectToWorldNormal(aTangent);
    v2f.bitangent = cross(aNormal, aTangent);

    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    vec3 cameraPos = ViewToWorldPos(vec3(0));
    vec3 viewDir = normalize(v2f.position - cameraPos);

    vec3 normal = normalize(v2f.normal);
    vec3 tangent = normalize(v2f.tangent);
    vec3 bitangent = normalize(v2f.bitangent);

    // Waves
    vec2 waveCoeffs = vec2(
    noise(v2f.uv * WaveScale + vec2(Core.time * 0.27, Core.time * 0.13) * WaveSpeed),
    noise(v2f.uv * WaveScale + vec2(Core.time * -0.17, Core.time * 0.07) * WaveSpeed)
) * WaveStrength;
    vec3 waveNormal = normalize(waveCoeffs.x * tangent + waveCoeffs.y * bitangent + (2-abs(waveCoeffs.x)-abs(waveCoeffs.y)) * normal);

    // Screen space refraction
    vec3 refractedRay = refract(viewDir, waveNormal, 1.0 / IOR);
    vec3 rayStart = v2f.position;
    vec3 rayDir = refractedRay;
    float rayLen = 0;
    float stepSize = 0.02;
    int i = 0;
    vec2 refractionUv;
    for (; i < MAX_DEPTH_STEPS; ++i)
    {
        rayLen += stepSize;
        stepSize *= 1.03;
        vec3 rayPos = rayStart + rayDir * rayLen;
        vec3 rayProjected = WorldPosToNormalizedViewport(rayPos);

        refractionUv = rayProjected.xy;

        float sampleDepth = SceneDepth(refractionUv);
        float depthDiff = DepthToLinear(rayProjected.z) - DepthToLinear(sampleDepth);

        if (depthDiff > 0 && depthDiff < DEPTH_THICKNESS)
            break;
    }
    if (i >= MAX_DEPTH_STEPS-2)
    {
        refractionUv = _ScreenPos;

        vec3 floorPos = WorldPosAtUvDepth(_ScreenPos, SceneDepth(_ScreenPos));
        float viewDist = length(floorPos - v2f.position);
        float refractedDist = viewDist * -viewDir.y / -refractedRay.y;
        rayLen = refractedDist;
    }

    // Blend screen edges
    vec2 screenEdgeUV = abs(refractionUv - 0.5);
    float screenEdgeFactor = max(screenEdgeUV.x, screenEdgeUV.y) * 2;
    screenEdgeFactor = RemapClamped(screenEdgeFactor, 0.85, 1, 0, 1);

    // Transmission
    vec3 transmissionCol = mix(SceneColor(refractionUv).rgb, SceneColor(_ScreenPos).rgb, screenEdgeFactor);
    transmissionCol = BeerLambertAbsorption(transmissionCol, AbsorptionsColor, rayLen*2);

    // Reflection
    vec3 reflectionRay = reflect(viewDir, waveNormal);
    vec3 reflectionCol = texture(_CubemapTex, reflectionRay).rgb;

    // Mix
    float reflectDot = dot(-viewDir, waveNormal);
    vec3 f0 = vec3((1 - IOR) / (1 + IOR));
    f0 = f0 * f0;
    float reflectance = FresnelShlick(reflectDot, f0).x;
    vec3 col = reflectance * reflectionCol + (1 - reflectance) * transmissionCol;

    FragOut = vec4(col, 1);
}