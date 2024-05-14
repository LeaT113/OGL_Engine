#include "Library/Core.glsl"
#include "Library/Tonemapping.glsl"
#include "Library/Optics.glsl"

#pragma DepthTest(Off)
#pragma DepthWrite(Off)

in vec3 aPosition;
in vec2 aTexCoord0;

uniform sampler2D SourceTex;
uniform sampler2D FogTex;

void vert()
{
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
}

void frag()
{
    vec3 col = texture(SourceTex, _ScreenPos).rgb;
    col += texture(FogTex, _ScreenPos).rgb;
    col = FilmicToneMapping(col);
    FragOut = vec4(col, 1.0);
}