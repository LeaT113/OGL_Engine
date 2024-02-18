vec3 UnpackNormal(vec3 packedNormal, float scale)
{
    vec3 normal;
    normal.xy = (packedNormal.xy * 2 - 1) * scale;
    normal.z = sqrt(1 - clamp(dot(normal.xy, normal.xy), 0, 1));
    return normal;
}