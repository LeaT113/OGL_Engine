#include "Library/Core.glsl"
#include "Library/Billboards.glsl"

#pragma DepthWrite(Off)
#pragma Blend(Add)

uniform sampler2D FlipbookTex;
uniform float Speed = 3;
uniform uint Dimension = 8;

in vec3 aPosition;
in vec2 aTexCoord0;

struct v2f
{
  vec2 uv;
};

void vert()
{
    v2f.uv = aTexCoord0;

    gl_Position = Billboard_ObjectToClipPos(aPosition);
}

void frag()
{
    vec2 scaledUv = v2f.uv / float(Dimension);

    float t = Core.time * Speed * Dimension;
    vec4 sample2 = texture(FlipbookTex, scaledUv + vec2(floor(t) / Dimension, -floor(t / Dimension) / Dimension));
    t -= 1;
    vec4 sample1 = texture(FlipbookTex, scaledUv + vec2(floor(t) / Dimension, -floor(t / Dimension) / Dimension));

    FragOut = mix(sample1, sample2, fract(t)) * 3;
}