float PointLight_DistanceAttenuationSimple(float distance)
{
    return 1 / (distance*distance);
}

float PointLight_DistanceAttenuationUnity(float distance, float range)
{
    // From https://catlikecoding.com/unity/tutorials/custom-srp/point-and-spot-lights/
    float d2r2 = (distance*distance)/(range*range);
    float f = max(0, 1 - (d2r2*d2r2));

    return f*f;
}

float PointLight_DistanceAttenuationSphere(float distance, float radius)
{
    // From http://www.cemyuksel.com/research/pointlightattenuation/
    return 2 / (distance*distance + radius*radius + distance*sqrt(distance*distance + radius*radius));
}