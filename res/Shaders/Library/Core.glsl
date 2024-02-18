uniform mat4 _ObjectToWorldMatrix;
uniform mat4 _WorldToObjectMatrix;
uniform mat3 _ObjectToWorldNormalMatrix;
uniform mat4 _WorldToViewMatrix;
uniform mat4 _ViewToWorldMatrix;
uniform mat4 _ObjectToClipMatrix;

uniform float _Time;
uniform vec3 _WorldSpaceCameraPos;


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
