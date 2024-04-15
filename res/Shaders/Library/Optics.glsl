vec3 BeerLambertAbsorption(vec3 color, vec3 absorptionColor, float length)
{
    return color * exp(-absorptionColor * length);
}

vec3 FresnelShlick(float cosTheta, vec3 F0)
{
    return F0 + (1 - F0) * pow(1 - cosTheta, 5);
}