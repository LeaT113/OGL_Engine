#include "Library/Core.glsl"
#include "Library/Tonemapping.glsl"

#pragma DepthTest(Off)
#pragma DepthWrite(Off)

in vec3 aPosition;
in vec2 aTexCoord0;

uniform sampler2D SourceTex;

struct v2f
{
  vec2 uv;
};

void vert()
{
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    v2f.uv = aTexCoord0;
}

void frag()
{
    vec3 col = texture(SourceTex, v2f.uv).rgb;
    col = FilmicToneMapping(col);
    FragOut = vec4(col, 1.0);
}