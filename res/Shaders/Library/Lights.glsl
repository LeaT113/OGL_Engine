#include "Library/Lighting.glsl"

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
    vec3 position;
};

struct SpotLight
{
    vec3 color;
    float spotA;
    vec3 position;
    float spotB;
    vec3 direction;
};


layout (std140, binding = 1) uniform lights
{
    AmbientLight ambientLight;
    DirectLight directLight;
    PointLight pointLights[4];
    SpotLight spotLights[4];
} Lights;


vec3 ApplyAmbientLight(vec3 position, vec3 normal, vec3 cameraPos, AmbientLight ambientLight)
{
    return ambientLight.color;
}

vec3 ApplyDirectLight(vec3 position, vec3 normal, vec3 cameraPos, DirectLight directLight)
{
    float angleAttenuation = max(0, dot(normal, -directLight.direction));

    return directLight.color * angleAttenuation;
}

vec3 ApplyPointLight(vec3 position, vec3 normal, vec3 cameraPos, PointLight pointLight)
{
    vec3 lightDir = normalize(pointLight.position - position);
    float angleAttenuation = max(0, dot(normal, lightDir));

    float dist = length(pointLight.position - position);
    float distanceAttenuation = Point_DistanceAttenuationSphere(dist, 0.1f);

    vec3 viewDir = normalize(cameraPos - position);
    vec3 halfDir = normalize(viewDir + lightDir);
    float specAttenuation = pow(max(dot(normal, halfDir), 0.0), 10);

    return pointLight.color * angleAttenuation * distanceAttenuation;
}

vec3 ApplySpotLight(vec3 position, vec3 normal, vec3 cameraPos, SpotLight spotLight)
{
    vec3 lightDir = normalize(spotLight.position - position);
    float angleAttenuation = max(0, dot(normal, lightDir));

    float dist = length(spotLight.position - position);
    float distanceAttenuation = Point_DistanceAttenuationSphere(dist, 0.1f);

    float spotDot = dot(-lightDir, spotLight.direction);
    float spotAttenuation = clamp(spotLight.spotA * spotDot + spotLight.spotB, 0, 1);

    return spotLight.color * angleAttenuation * distanceAttenuation * spotAttenuation;
}

vec3 ApplyLights(vec3 position, vec3 normal, vec3 cameraPos)
{
    vec3 l = vec3(0.0);

    l += ApplyAmbientLight(position, normal, cameraPos, Lights.ambientLight);
    l += ApplyDirectLight(position, normal, cameraPos, Lights.directLight);

    for (int i = 0; i < 3; i++)
    {
        l += ApplyPointLight(position, normal, cameraPos, Lights.pointLights[i]);
        l += ApplySpotLight(position, normal, cameraPos, Lights.spotLights[i]);
    }
    return l;
}