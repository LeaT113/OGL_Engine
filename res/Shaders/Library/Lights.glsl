#include "Library/Lighting.glsl"

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