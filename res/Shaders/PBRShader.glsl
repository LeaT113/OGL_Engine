#include "Library/Core.glsl"
#include "Library/Shadows.glsl"
#include "Library/Lights.glsl"
#include "Library/Tonemapping.glsl"
#include "Library/NormalMapping.glsl"
#include "Library/HeightMapping.glsl"
#include "Library/Scene.glsl"

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTexCoord0;

uniform vec4 Color;
uniform float TextureScale = 2.0;
uniform sampler2D AlbedoTex;
uniform sampler2D RoughnessTex;
uniform sampler2D OcclusionTex;
uniform sampler2D NormalTex;
uniform bool UseNormalmap;
uniform float NormalMapStrenght;
uniform sampler2D HeightmapTex;
uniform bool UseHeightmap;
uniform float HeightmapDepth;

struct v2f
{
    vec3 position;
    vec3 normal;
    vec2 uv;

    mat3 tbn;
};

void vert()
{
    v2f.position = ObjectToWorldPos(aPosition);
    v2f.normal = ObjectToWorldNormal(aNormal);
    v2f.uv = aTexCoord0;
    v2f.tbn = CreateTBNMatrix(aNormal, aTangent);

    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    vec3 positionWS = v2f.position;
    vec3 originalPosWS = v2f.position;
    vec3 normalWS = normalize(v2f.normal);
    vec3 cameraPos = ViewToWorldPos((0).xxx);
    vec2 uv = v2f.uv * TextureScale;

    if(UseHeightmap)
    {
        vec3 viewTS = normalize(inverse(v2f.tbn) * (positionWS - cameraPos));
        vec3 parallaxOffset = ParallaxOcclusionMapping_Offset(HeightmapTex, uv, viewTS, HeightmapDepth);

        uv += parallaxOffset.xy;
        positionWS += v2f.tbn * parallaxOffset;
    }

    if (UseNormalmap)
    {
        vec3 normal = texture(NormalTex, uv).rgb;
        normal = UnpackNormal(normal, NormalMapStrenght);
        normalWS = normalize(v2f.tbn * normal);
    }

    // Shading
    vec4 albedo = texture(AlbedoTex, uv);
    float occlusion = texture(OcclusionTex, uv).r;
    vec3 lighting = ApplyLights(originalPosWS, normalWS, cameraPos);

    vec3 col = albedo.rgb * lighting * occlusion * occlusion * occlusion;
    color = vec4(col, 1);
}
