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

float DistributionGGX(float roughness, float nDotH)
{
    //                        a^4
    // D = --------------------------------------------
    //     PI * (dot(normal, half)^2 * (a^4 - 1) + 1)^2

    float alpha4 = pow(roughness, 4.0);
    float d = (nDotH * nDotH * (alpha4 - 1.0) + 1.0);
    return alpha4 / (PI * d * d);
}

float GeometrySchlickGGX(float roughness, float dot)
{
    //     (roughness + 1)^2
    // k = -----------------
    //            8
    //           n·v
    // G = ----------------
    //     (n·v)(1 - k) + k

    float r = roughness + 1.0;
    float k = r * r
                /
               8.0;

    return   dot
              /
    (dot * (1.0 - k) + k);
}

float GeometrySmith(float roughness, float nDotV, float nDotL)
{
    return GeometrySchlickGGX(roughness, nDotV) * GeometrySchlickGGX(roughness, nDotL);
}

vec3 FresnelSchlick(vec3 f0, float vDotH)
{
    // F = F_0 + (1 - F_0) * (1 - dot(view, half))^5

    return f0 + (1.0 - f0) * pow(saturate(1.0 - vDotH), 5.0);
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
    float D = DistributionGGX(surface.roughness, nDotH);
    float G = GeometrySmith(surface.roughness, nDotV, nDotL);
    vec3 f0 = mix(DIELECTRIC_F0, surface.color, surface.metallic);
    vec3 F = FresnelSchlick(f0, vDotH);

    vec3 SpecularBRDF =         D * G * F
                                    /
                       4.0 * nDotV * nDotL + 0.0001;

    // Diff
    vec3 kD = 1.0 - F;
    vec3 lambert = surface.color * (1.0 - surface.metallic);
    vec3 DiffuseBRDF = kD * lambert * surface.occlusion / PI;

    return (DiffuseBRDF + SpecularBRDF) * lightEnergy * nDotL;
}