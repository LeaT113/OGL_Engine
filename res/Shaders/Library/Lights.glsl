#include "Library/Lighting.glsl"

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct AmbientLight
{
  vec3 color;
};

struct DirectLight
{
    vec3 color;
    vec3 direction;
};

struct PointLight
{
    vec3 color;
    int shadowIndex;
    vec3 position;
};

struct SpotLight
{
    vec3 color;
    float spotA;
    vec3 position;
    float spotB;
    vec3 direction;
    int shadowIndex;
};


layout (std140, binding = 2) uniform lights
{
    AmbientLight ambientLight;
    DirectLight directLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
} Lights;


vec3 PointLight_Energy(vec3 position, vec3 normal, PointLight light)
{
    vec3 lightDir = normalize(light.position - position);
    float angleAtt = max(dot(normal, lightDir), 0);

    float dist = length(light.position - position);
    float distAtt = PointLight_DistanceAttenuationSphere(dist, 0.1f);

    return light.color * angleAtt * distAtt;
}

vec3 SpotLight_Energy(vec3 position, vec3 normal, SpotLight light)
{
    vec3 lightDir = normalize(light.position - position);
    float angleAtt = max(dot(normal, lightDir), 0);

    float dist = length(light.position - position);
    float distAtt = PointLight_DistanceAttenuationSphere(dist, 0.1f);

    float spotDot = dot(-lightDir, light.direction);
    float spotAtt = clamp(spotDot * light.spotA + light.spotB, 0, 1);
    spotAtt = spotAtt * spotAtt;

    return light.color * angleAtt * distAtt * spotAtt;
}
