#include "Library/Core.glsl"
#include "Library/Scene.glsl"
#include "Library/Lights.glsl"
#include "Library/Shadows.glsl"
#include "Library/Optics.glsl"

#pragma DepthTest(Off)
#pragma DepthWrite(Off)
#define ABSORB vec3(0.85)
uniform vec3 AbsorptionColor = vec3(50);

vec3 ViewPosToNormalizedViewport(vec3 viewPos)
{
    vec4 projectedPos = _ProjectionMatrix * vec4(viewPos, 1);
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
    vec3 rayDir = normalize((_InvViewMatrix * _InvProjectionMatrix * vec4(v2f.pos.xy, 1, 1.0)).xyz);
    float stopDepth = DepthToLinear(SceneDepth(WorldPosToNormalizedViewport(rayDir).xy));

    float stepSize = 0.1;
    vec3 rayStart = ViewToWorldPos(vec3(0));
    float rayLen = 0;
    vec3 totalLight = vec3(0);
    for (int i = 0; i < 50; i++)
    {
        rayLen += stepSize;
        stepSize *= 1.01;
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
            vec3 lightEnergy = PointLight_Energy(rayPos, normalize(dir), light);
            #ifdef SHADOWS
                lightEnergy *= PointLight_Shadow(rayPos, light);
            #endif

            // Attentuate distance to light
            collectedLight += BeerLambertAbsorption(lightEnergy, ABSORB, length(dir));
        }

        totalLight += BeerLambertAbsorption(collectedLight, ABSORB, rayLen);
    }

//    float depth = DepthToLinear(SceneDepth(posClipSpace.xy * 0.5 + 0.5));

//    col = BeerLambertAbsorption(col, vec3(0.001), depth);

    vec3 col = SceneColor(_ScreenPos).xyz;
    col += totalLight;

    FragOut = vec4(col, 1.0);
}