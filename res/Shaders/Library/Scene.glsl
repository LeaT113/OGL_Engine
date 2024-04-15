float DepthToLinear(float depth)
{
    float z = depth * 2 - 1;
    float near = Core.cameraNearFar.x;
    float far = Core.cameraNearFar.y;
    return (2.0 * near * far) / (far + near - z * (far - near));;
}

vec2 WorldPosToUV(vec3 worldPos)
{
    vec4 projected = _ViewToClip * _WorldToViewMatrix * vec4(worldPos, 1);
    projected.xyz /= projected.w;
    return projected.xy * 0.5 + 0.5;
}

float WorldPosToFragZ(vec3 worldPos)
{
    vec4 clipPos = _ViewToClip * _WorldToViewMatrix * vec4(worldPos, 1);
    float ndc_z = clipPos.z / clipPos.w;
    return ndc_z * 0.5 + 0.5;
}

vec3 WorldPosAtUvDepth(vec2 uv, float depth)
{
    float ndcDepth = depth * 2 - 1;
    vec2 ndcXY = uv * 2 - 1;
    vec4 ndcPos = vec4(ndcXY, ndcDepth, 1.0);

    vec4 cameraPos = _ClipToView * ndcPos;
    cameraPos /= cameraPos.w;

    vec4 worldPos = _ViewToWorldMatrix * cameraPos;

    return worldPos.xyz;
}