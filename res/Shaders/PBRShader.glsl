#include "Library/Core.glsl"
#include "Library/Lights.glsl"
#include "Library/Shadows.glsl"
#include "Library/NormalMapping.glsl"
#include "Library/HeightMapping.glsl"
#include "Library/PBR.glsl"

uniform vec3 Color = vec3(1, 1, 1);
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

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTexCoord0;

in mat4 aInstanceTransform;

struct v2f
{
    vec3 position;
    vec3 normal;
    vec2 uv;

    mat3 tbn;
};

void vert()
{
    if (_Instanced)
    {
        vec4 pos = aInstanceTransform * vec4(aPosition, 1);
        v2f.position = pos.xyz;

        v2f.normal = normalize((aInstanceTransform * vec4(aNormal, 0)).xyz);
        vec3 T = normalize((aInstanceTransform * vec4(aTangent, 0)).xyz);
        vec3 B = cross(v2f.normal, T);
        v2f.tbn = mat3(T, B, v2f.normal);

        gl_Position = _ProjectionMatrix * _ViewMatrix * pos;
    }
    else
    {
        v2f.position = ObjectToWorldPos(aPosition);
        v2f.normal = ObjectToWorldNormal(aNormal);
        v2f.tbn = CreateTBNMatrix(aNormal, aTangent);
        gl_Position = ObjectToClipPos(aPosition);
    }

    v2f.uv = aTexCoord0;
}

void frag()
{
    vec3 positionWS = v2f.position;
    vec3 cameraPos = ViewToWorldPos((0).xxx);
    vec3 viewDir = normalize(cameraPos - positionWS);
    vec2 uv = v2f.uv * TextureScale;

    vec3 originalPosWS = v2f.position;
    if(UseHeightmap)
    {
        vec3 viewTS = normalize(inverse(v2f.tbn) * (positionWS - cameraPos));
        vec3 parallaxOffset = ParallaxOcclusionMapping_Offset(HeightmapTex, uv, viewTS, HeightmapDepth);

        uv += parallaxOffset.xy;
        positionWS += v2f.tbn * parallaxOffset;
        viewDir = normalize(cameraPos - positionWS);
    }

    vec3 normalWS = normalize(v2f.normal);
    if (UseNormalmap)
    {
        vec3 normal = texture(NormalTex, uv).rgb;
        normal = UnpackNormal(normal, NormalMapStrenght);
        normalWS = normalize(v2f.tbn * normal);
    }

    // Textures
    vec3 albedo = texture(AlbedoTex, uv).rgb * Color;
    float roughness = texture(RoughnessTex, uv).r;
    float occlusion = texture(OcclusionTex, uv).r;
    occlusion *= occlusion;

    // Shading
    Surface surface = Surface(albedo, roughness, 0, occlusion, normalWS);
    vec3 brdf = vec3(0);

    // TODO Ambient and direct
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        PointLight light = Lights.pointLights[i];
        vec3 lightEnergy = PointLight_Energy(positionWS, normalWS, light);
        #ifdef SHADOWS
            lightEnergy *= PointLight_Shadow(originalPosWS, light);
        #endif

        brdf += BRDF(surface, viewDir, lightEnergy, normalize(light.position - positionWS));
    }
    for (int i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        SpotLight light = Lights.spotLights[i];
        vec3 lightEnergy = SpotLight_Energy(positionWS, normalWS, light);
        #ifdef SHADOWS
            lightEnergy *= SpotLight_Shadow(originalPosWS, light);
        #endif

        brdf += BRDF(surface, viewDir, lightEnergy, normalize(light.position - positionWS));
    }

//    brdf += BRDF(surface, viewDir, vec3(3), vec3(0, 1, 0));

    FragOut = vec4(brdf, 1);
}
