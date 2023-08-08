vec3 PointLight(vec3 position, vec3 normal, vec3 lightPosition, float lightRadius, vec3 lightColor)
{
    vec3 lightDir = normalize(lightPosition - position);
    float angleAttenuation = max(0, dot(normal, lightDir));

    float dist = length(lightPosition - position);
    float distanceAttenuation = Point_DistanceAttenuationSphere(dist, lightRadius);

    return lightColor * angleAttenuation * distanceAttenuation;
}