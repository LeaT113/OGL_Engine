#include "Library/Lighting.glsl"

struct sPointLight
{
    vec3 position;
    vec3 color;
    float radius;
};

layout (std140, binding = 1) uniform lights
{
    sPointLight pointLights[4];
} Lights;


vec3 PointLight(vec3 position, vec3 normal, vec3 cameraPos, vec3 lightPosition, float lightRadius, vec3 lightColor)
{
    vec3 lightDir = normalize(lightPosition - position);
    float angleAttenuation = max(0, dot(normal, lightDir));

    float dist = length(lightPosition - position);
    float distanceAttenuation = Point_DistanceAttenuationSphere(dist, lightRadius);

    vec3 viewDir = normalize(cameraPos - position);
    vec3 halfDir = normalize(viewDir + lightDir);

    float specAttenuation = pow(max(dot(normal, halfDir), 0.0), 10);


    return lightColor * angleAttenuation * distanceAttenuation + lightColor * specAttenuation;
}

vec3 ApplyLights(vec3 position, vec3 normal, vec3 cameraPos)
{
    vec3 l = vec3(0.0);
    for (int i = 0; i < 3; i++)
    {
        l += PointLight(position, normal, cameraPos, Lights.pointLights[i].position, Lights.pointLights[i].radius, Lights.pointLights[i].color);
    }
    return l;
}