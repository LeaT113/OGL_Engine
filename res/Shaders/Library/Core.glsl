uniform mat4 _ObjectToWorldMatrix;
uniform mat4 _WorldToObjectMatrix;
uniform mat3 _ObjectToWorldNormalMatrix;
uniform mat4 _WorldToViewMatrix;
uniform mat4 _ViewToWorldMatrix;
uniform mat4 _ObjectToClipMatrix;
uniform mat4 _ViewToClip;
uniform mat4 _ClipToView;

layout (std140, binding = 1) uniform core
{
    vec2 screenSize;
    vec2 cameraNearFar;
    float time;
} Core;

uniform vec3 _WorldSpaceCameraPos;
uniform sampler2D _SceneColorTex;
uniform sampler2D _SceneDepthTex;
uniform samplerCube _CubemapTex;

#define _ScreenPos (gl_FragCoord.xy/Core.screenSize)


//          Transformations
// Position
vec3 ObjectToWorldPos(vec3 pos)
{
    return (_ObjectToWorldMatrix * vec4(pos, 1.0)).xyz;
}
vec3 WorldToObjectPos(vec3 pos)
{
    return (_WorldToObjectMatrix * vec4(pos, 1.0)).xyz;
}

vec3 WorldToViewPos(vec3 pos)
{
    return (_WorldToViewMatrix * vec4(pos, 1.0)).xyz;
}
vec3 ViewToWorldPos(vec3 pos)
{
    return (_ViewToWorldMatrix * vec4(pos, 1.0)).xyz;
}

vec4 ObjectToClipPos(vec3 pos)
{
    return _ObjectToClipMatrix * vec4(pos, 1.0);
}

// Normal
vec3 ObjectToWorldNormal(vec3 normal)
{
    // https://zhangdoa.com/normal-and-normal-mapping
    return normalize(_ObjectToWorldNormalMatrix * normal);
}

// Mapping
float saturate(float value)
{
    return clamp(value, 0, 1);
}
float Remap(float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}
float RemapClamped(float value, float min1, float max1, float min2, float max2)
{
    return clamp(Remap(value, min1, max1, min2, max2), 0.0, 1.0);
}