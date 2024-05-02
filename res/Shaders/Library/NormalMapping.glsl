#include "Library/Core.glsl"

mat3 CreateTBNMatrix(vec3 normal, vec3 tangent)
{
    vec3 N = ObjectToWorldNormal(normal);
    vec3 T = ObjectToWorldNormal(tangent);
    vec3 B = cross(N, T);
    return mat3(T, B, N);
}

vec3 UnpackNormal(vec3 packedNormal, float scale)
{
    vec3 normal;
    normal.xy = (packedNormal.xy * 2 - 1) * scale;
    normal.z = sqrt(1 - clamp(dot(normal.xy, normal.xy), 0, 1));
    return normal;
}