#include "Library/Core.glsl"
#include "Library/Noise.glsl"
#include "Library/Billboards.glsl"
#include "Library/Scene.glsl"

#pragma Blend(Add)
#pragma DepthWrite(Off)
#pragma DepthTest(Off)
#pragma Cull(Off)

in vec3 aPosition;
in vec3 aTexCoord0;

uniform float Intensity = 5;
uniform float Threshold = 4.06;
uniform float Smoothness = 5;
uniform sampler2D FireNoiseTex;

struct v2f
{
    vec2 uv;
    float depth;
};

void vert()
{
    v2f.uv = aTexCoord0.xy;

    vec4 clipPos = CylinderBillboard_ObjectToClipPos(aPosition);
    gl_Position = clipPos;
    v2f.depth = clipPos.z / clipPos.w;
}

void frag()
{
    vec2 uv = 1-v2f.uv.yx;
    uv *= 0.8;

    float texture1 = texture(FireNoiseTex, uv * 2 * vec2(0.7, 0.8) + vec2(0.5, 1) * 0.8 * Core.time).r;
    float texture2 = texture(FireNoiseTex, uv * 2 * vec2(0.5, 1) + vec2(0.3, -0.1) * 0.6 * Core.time).r;

    float border1 = texture(FireNoiseTex, uv * 0.8 * vec2(0.7, 1.4) + vec2(0.5, 0) * 0.7 * Core.time).r;
    float border2 = texture(FireNoiseTex, uv * 0.8 * vec2(1, 1.8) + vec2(0.7, 0) * 0.6 * Core.time).r;
    float border = border1 * border2;
    border += 1 - uv.x;
    border *= 1 - uv.x;
    border *= Intensity;
    float shapeNoise = noise(uv * vec2(5, 8) + vec2(1.5, 0.12) * Core.time * 2);
    border += pow(abs(v2f.uv.x - 0.5)*2, 1.5) * 5;
    border += shapeNoise * pow((1 - abs(v2f.uv.x - 0.5))*2, 2) * 0.6;
    border = Remap(border, Threshold - Smoothness, Threshold, 1, 0);
    border = smoothstep(0, 1, border);
    border = pow(border, 2);

    float val = texture1 * texture2 * border;
    val = saturate(val);
    val = pow(val, 1.2);

    // Depth fade
    float sampleDepth = DepthToLinear(SceneDepth(_ScreenPos));
    float depth = NDC_ZToLinear(v2f.depth);
    float depthDiff = depth - sampleDepth + 0.1;
    float fade = RemapClamped(depthDiff, 0, 0.5, 1, 0);

    FragOut = vec4(val * vec3(0.761, 0.2, 0.05) * 255 * fade, 1);
}