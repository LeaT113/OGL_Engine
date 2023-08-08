float Fresnel(vec3 viewDir, vec3 normal)
{
    return clamp(pow(dot(-viewDir, normal), 5), 0, 1);
}