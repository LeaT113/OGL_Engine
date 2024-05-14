#include "Library/Core.glsl"

uniform sampler2D _SceneColorTex;
uniform sampler2D _SceneDepthTex;


vec4 SceneColor(vec2 uv)
{
    return texture(_SceneColorTex, uv);
}

float SceneDepth(vec2 uv)
{
    return texture(_SceneDepthTex, uv).r;
}

float DepthToLinear(float depth)
{
    float near = Core.cameraNearFar.x;
    float far = Core.cameraNearFar.y;
    return 2 * far * near / (far + near - (far - near) * (2 * depth - 1));

    float z = depth * 2 - 1;
    return (2.0 * near * far) / (far + near - z * (far - near));;
}

vec3 WorldPosToNormalizedViewport(vec3 worldPos)
{
    // Returns position where xy is ScreenPos and z is Depth
    vec4 projectedPos = _ProjectionMatrix * _ViewMatrix * vec4(worldPos, 1);
    projectedPos.xyz /= projectedPos.w;
    return projectedPos.xyz * 0.5 + 0.5;
}

vec3 WorldPosAtUvDepth(vec2 uv, float depth)
{
    float ndcDepth = depth * 2 - 1;
    vec2 ndcXY = uv * 2 - 1;
    vec4 ndcPos = vec4(ndcXY, ndcDepth, 1.0);

    vec4 cameraPos = _InvProjectionMatrix * ndcPos;
    cameraPos /= cameraPos.w;

    vec4 worldPos = _InvViewMatrix * cameraPos;

    return worldPos.xyz;
}