#define PI 3.1415926535897932384626433832795

layout (std140, binding = 1) uniform core
{
    vec2 screenSize;
    vec2 cameraNearFar;
    float time;
} Core;

uniform vec3 _WorldSpaceCameraPos;
uniform samplerCube _CubemapTex;
#define _ScreenPos (gl_FragCoord.xy/Core.screenSize)


//          Transformations
uniform mat4 _ModelMatrix;
uniform mat4 _InvModelMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _InvViewMatrix;
uniform mat4 _ProjectionMatrix;
uniform mat4 _InvProjectionMatrix;
uniform mat3 _ModelNormalMatrix;
uniform mat4 _MVPMatrix;

uniform bool _Instanced;

vec3 ObjectToWorldPos(vec3 pos)
{
    return (_ModelMatrix * vec4(pos, 1.0)).xyz;
}
vec3 WorldToObjectPos(vec3 pos)
{
    return (_InvModelMatrix * vec4(pos, 1.0)).xyz;
}

vec3 WorldToViewPos(vec3 pos)
{
    return (_ViewMatrix * vec4(pos, 1.0)).xyz;
}
vec3 ViewToWorldPos(vec3 pos)
{
    return (_InvViewMatrix * vec4(pos, 1.0)).xyz;
}

vec4 ObjectToClipPos(vec3 pos)
{
    return _MVPMatrix * vec4(pos, 1.0);
}

// Normal
vec3 ObjectToWorldNormal(vec3 normal)
{
    // https://zhangdoa.com/normal-and-normal-mapping
    return normalize(_ModelNormalMatrix * normal);
}

float NDC_ZToLinear(float z)
{
    float near = Core.cameraNearFar.x;
    float far = Core.cameraNearFar.y;
    return 2 * far * near / (far + near - (far - near) * z);
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