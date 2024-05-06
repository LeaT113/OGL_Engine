#include "Library/Core.glsl"

vec4 Billboard_ObjectToClipPos(vec3 pos)
{
    mat4 modelViewMatrix = _ViewMatrix * _ModelMatrix;

    modelViewMatrix[0][0] = 1.0;
    modelViewMatrix[0][1] = 0.0;
    modelViewMatrix[0][2] = 0.0;

    modelViewMatrix[1][0] = 0.0;
    modelViewMatrix[1][1] = 1.0;
    modelViewMatrix[1][2] = 0.0;

    modelViewMatrix[2][0] = 0.0;
    modelViewMatrix[2][1] = 0.0;
    modelViewMatrix[2][2] = 1.0;

    return _ProjectionMatrix * modelViewMatrix * vec4(pos, 1);
}

vec4 CylinderBillboard_ObjectToClipPos(vec3 pos)
{
    mat4 modelViewMatrix = _ViewMatrix * _ModelMatrix;

    modelViewMatrix[0][0] = 1.0;
    modelViewMatrix[0][1] = 0.0;
    modelViewMatrix[0][2] = 0.0;

    modelViewMatrix[2][0] = 0.0;
    modelViewMatrix[2][1] = 0.0;
    modelViewMatrix[2][2] = 1.0;

    return _ProjectionMatrix * modelViewMatrix * vec4(pos, 1);
}