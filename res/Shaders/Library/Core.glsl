uniform mat4 ObjectToWorldMatrix;
uniform mat4 WorldToObjectMatrix;
uniform mat4 WorldToViewMatrix;
uniform mat4 ViewToWorldMatrix;
uniform mat4 ObjectToClipMatrix;

// Position
vec3 ObjectToWorldPos(vec3 pos)
{
    return (ObjectToWorldMatrix * vec4(pos, 1.0)).xyz;
}
vec3 WorldToObjectPos(vec3 pos)
{
    return (WorldToObjectMatrix * vec4(pos, 1.0)).xyz;
}

vec3 WorldToViewPos(vec3 pos)
{
    return (WorldToViewMatrix * vec4(pos, 1.0)).xyz;
}
vec3 ViewToWorldPos(vec3 pos)
{
    return (ViewToWorldMatrix * vec4(pos, 1.0)).xyz;
}

vec4 ObjectToClipPos(vec3 pos)
{
    return ObjectToClipMatrix * vec4(pos, 1.0);
}

// Normal
vec3 ObjectToWorldNormal(vec3 normal)
{
    // TODO Check non-uniform scaling
    return normalize(
        WorldToObjectMatrix[0].xyz * normal.x +
        WorldToObjectMatrix[1].xyz * normal.y +
        WorldToObjectMatrix[2].xyz * normal.z
    );
}
