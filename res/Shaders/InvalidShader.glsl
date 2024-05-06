#include "Library/Core.glsl"
#include "Library/Blending.glsl"
#include "Library/Tonemapping.glsl"

#define SCALE 30

in vec3 aPosition;
in vec3 aNormal;


float BluredWave(float x, float blur)
{
    float wave = cos(x);

    float inBound = 0.05 * (1.0 + blur*50.0);
    float outBound = RemapClamped(blur, 0.6, 1.0, 0.0, 0.5);

    return  RemapClamped(wave, -inBound, inBound, outBound, 1.0 - outBound);
}

float Wave(float x)
{
    return RemapClamped(sin(x), -1.0, 1.0, 0.0, 1.0);
}

float CombineWaves(float wave1, float wave2)
{
    return wave1 + wave2 - 2.0*wave1*wave2;
}
float CombineWaves3(float wave1, float wave2, float wave3)
{
    float xz = CombineWaves(wave1, wave2);
    float xyz = CombineWaves(xz, wave3);
    return xyz / 1.5;
}

float MapWaves(float checkerBase, float blur)
{
    float inBound = 0.5 - (0.003 + blur*5.0);
    float outBound = RemapClamped(blur, 0.6, 1.0, 0.0, 0.5);

    return RemapClamped(checkerBase, inBound, 1.0-inBound, outBound, 1.0 - outBound);
}



struct v2f
{
    vec3 position;
    vec3 normal;
    vec4 positionCS;
};

void vert()
{
    v2f.position = ObjectToWorldPos(aPosition);
    v2f.normal = ObjectToWorldNormal(aNormal);

    gl_Position = ObjectToClipPos(aPosition);
    v2f.positionCS = gl_Position;
}

void frag()
{
    float distanceFactor = RemapClamped(v2f.positionCS.w, 2, 15, 0, 1);
    distanceFactor *= distanceFactor;

    vec3 cameraPos = ViewToWorldPos((0).xxx);
    vec3 viewDir = normalize(cameraPos - v2f.position);
    float angle = 1 - max(0, dot(normalize(v2f.normal), viewDir));
    //angle = pow(angle, 5);

    vec3 scaledPos = v2f.position * SCALE;

    // Calculate blur factor
    float dxPos = length(dFdx(scaledPos*2));
    float dyPos = length(dFdy(scaledPos*2));
    float dPosMax = max(dxPos, dyPos);
    float mipmap = clamp(log2(dPosMax), 0, 7);
    float blurFactor = pow(mipmap / 7, 0.6);
    //float blurFactor = pow(saturate(dPosMax*4 - 0.05), 0.5);

    // Generate checker mask

    //float checker = CombineWaves3(Wave(scaledPos.x), Wave(scaledPos.z), Wave(scaledPos.y));//, Wave(scaledPos.z));
    //checker = MapWaves(checker, blurFactor);
    float checker = Blend_Exclusion(Blend_Exclusion(BluredWave(scaledPos.x, blurFactor).xxx, BluredWave(scaledPos.y, blurFactor).xxx), BluredWave(scaledPos.z, blurFactor).xxx).x;

    vec3 col = mix(vec3(1, 0, 1), vec3(0,0,0), checker);
    col = pow(col, (1/2.2).xxx);
    FragOut = vec4(col, 1.0);
    //color = vec4((floor(blurFactor*10)/10).xxx, 1.0);
}