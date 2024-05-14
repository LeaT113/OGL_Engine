#include "Library/Core.glsl"
#include "Library/Scene.glsl"
#include "Library/Lights.glsl"
#include "Library/Shadows.glsl"
#include "Library/Optics.glsl"

#pragma DepthTest(Off)
#pragma DepthWrite(Off)
#define ABSORB vec3(0.1)
#define LIGHT_FACTOR 0.005
#define MAX_STEPS 100

vec3 WorldToNormalizedViewportPos(vec4 viewPos)
{
    vec4 projectedPos = _ProjectionMatrix * viewPos;
    projectedPos.xyz /= projectedPos.w;
    return projectedPos.xyz * 0.5 + 0.5;
}

in vec3 aPosition;
in vec2 aTexCoord0;

struct v2f
{
    vec3 pos;
};

void vert()
{
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    v2f.pos = vec3(aPosition.x, aPosition.y, 0);
}

void frag()
{
    vec4 viewPos = _InvProjectionMatrix * vec4(v2f.pos.xy, 1.0, 1.0);
    vec3 rayDir = normalize((_InvViewMatrix * vec4(viewPos.xyz, 0)).xyz);
    float stopDepth = DepthToLinear(SceneDepth(_ScreenPos));

    float stepSize = 0.1;
    vec3 rayStart = ViewToWorldPos(vec3(0));
    float rayLen = 0.1;
    vec3 totalLight = vec3(0);
    for (int i = 0; i < MAX_STEPS; i++)
    {
        rayLen += stepSize;
        stepSize *= 1.04;
        vec3 rayPos = rayStart + rayDir * rayLen;
        vec3 projected = WorldPosToNormalizedViewport(rayPos);
        if (DepthToLinear(projected.z) > stopDepth)
            break;

        // Lights
        vec3 collectedLight = vec3(0);
        for (int i = 0; i < MAX_POINT_LIGHTS; i++)
        {
            PointLight light = Lights.pointLights[i];
            vec3 dir = light.position - rayPos;
            vec3 lightEnergy = PointLight_Energy(rayPos, normalize(dir), light) * LIGHT_FACTOR;
            lightEnergy *= RemapClamped(distance(light.position, rayStart), 1, 5, 0.05, 1);
            #ifdef SHADOWS
                lightEnergy *= PointLight_Shadow(rayPos, light);
            #endif

            // Attentuate distance to light
            collectedLight += BeerLambertAbsorption(lightEnergy, ABSORB, length(dir));
        }
        for (int i = 0; i < MAX_SPOT_LIGHTS; i++)
        {
            SpotLight light = Lights.spotLights[i];
            vec3 dir = light.position - rayPos;
            vec3 lightEnergy = SpotLight_Energy(rayPos, normalize(dir), light) * LIGHT_FACTOR;
            lightEnergy *= RemapClamped(distance(light.position, rayStart), 1, 3, 0.2, 1);
            #ifdef SHADOWS
                lightEnergy *= SpotLight_Shadow(rayPos, light);
            #endif

            collectedLight += BeerLambertAbsorption(lightEnergy, ABSORB, length(dir));
        }

        totalLight += BeerLambertAbsorption(collectedLight, ABSORB, rayLen);
    }

    FragOut = vec4(totalLight, 1.0);
}