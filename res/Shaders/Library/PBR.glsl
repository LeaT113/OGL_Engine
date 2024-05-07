#include "Library/Core.glsl"

#define DIELECTRIC_F0 vec3(0.04)

struct Surface
{
    vec3 color;
    float roughness;
    float metallic;
    float occlusion;
    vec3 normal;
};

// Utils
vec3 HalfVector(vec3 v1, vec3 v2)
{
    return normalize(v1 + v2);
}

float Distribution_GGX(float roughness, float nDotH)
{
    //                        a^4
    // D = --------------------------------------------
    //     PI * (dot(normal, half)^2 * (a^4 - 1) + 1)^2

    float alpha4 = pow(roughness, 4);
    float d = (nDotH * nDotH * (alpha4 - 1) + 1);
    return alpha4 / (PI * d * d);
}

float Geom_Smith(float roughness, float dotProduct)
{
    // Shlick GGX
    float k = (roughness + 1) * (roughness + 1 ) / 8;
    return dotProduct / (dotProduct * (1-k) + k);
}

vec3 Fresnel_Schlick(vec3 f0, float vDotH)
{
    // F = F_0 + (1 - F_0) * (1 - dot(view, half))^5

    return f0 + (1 - f0) * pow(saturate(1.0 - vDotH), 5);
}

// BRDF
vec3 BRDF(Surface surface, vec3 viewDir, vec3 lightEnergy, vec3 lightDir)
{
    vec3 halfVec = HalfVector(viewDir, lightDir);

    float nDotL = max(dot(surface.normal, lightDir), 0);
    float nDotV = max(dot(surface.normal, viewDir), 0);
    float nDotH = max(dot(surface.normal, halfVec), 0);
    float vDotH = max(dot(viewDir, halfVec), 0);

    //                D * G * F
    // Spec = -------------------------
    //        4 * dot(N, L) * dot(N, V)
    float D = Distribution_GGX(surface.roughness, nDotH);
    float G = Geom_Smith(surface.roughness, nDotL) * Geom_Smith(surface.roughness, nDotV);
    vec3 f0 = mix(DIELECTRIC_F0, surface.color, surface.metallic);
    vec3 F = Fresnel_Schlick(f0, vDotH);

    vec3 SpecularBRDF =         D * G * F
                                    /
                       4 * nDotV * nDotL + 0.0001;

    // Diff
    vec3 kD = 1.0 - F;
    vec3 lambert = mix(surface.color, vec3(0), surface.metallic);
    vec3 DiffuseBRDF = kD * lambert * surface.occlusion / PI;

    return (DiffuseBRDF + SpecularBRDF) * lightEnergy * nDotL;
}