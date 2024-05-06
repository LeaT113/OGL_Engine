#include "Library/Core.glsl"

#pragma DepthTest(Off)
#pragma DepthWrite(Off)
#pragma Blend(Alpha)

uniform usampler2D EntityIds;
uniform uint Id;
uniform vec4 Color = vec4(0.9, 0.5, 0.25, 0.3);

in vec3 aPosition;

void vert()
{
    gl_Position = ObjectToClipPos(aPosition);
}

void frag()
{
    uint sampleId = texture(EntityIds, _ScreenPos).r;

    FragOut = vec4(Color.rgb, Color.a * Remap(sin(Core.time * 5), -1, 1, 0.2, 1));
}