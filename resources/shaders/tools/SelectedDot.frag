#include "../GlobalDataBuffer.glsl"

layout (set = 0, binding = 1) uniform sampler2D selectionIdSampler;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Push {
    mat4 model;
    vec4 selectionColor; // xyz: color, w: thickness
    uint renderIndex;
} push;

void main() {
    uint currentIndex = uint(abs(texelFetch(selectionIdSampler, ivec2(gl_FragCoord.xy), 0).r));
    if (currentIndex == 0) {
        discard;
    }
    int thickness = int(push.selectionColor.a);
    bool isBoundary = false;

    ivec2 pixel = ivec2(gl_FragCoord.xy);
    ivec2 texSize = textureSize(selectionIdSampler, 0);

    for (int y = -thickness; y <= thickness; ++y) {
        for (int x = -thickness; x <= thickness; ++x) {

            if (x == 0 && y == 0)
            continue;

            ivec2 neighborPixel = pixel + ivec2(x, y);

            if (neighborPixel.x < 0 || neighborPixel.x >= int(texSize.x) ||
            neighborPixel.y < 0 || neighborPixel.y >= int(texSize.y)) {
                isBoundary = true;
                break;
            }

            uint neighborIndex = uint(abs(texelFetch(selectionIdSampler, neighborPixel, 0).r));

            if (neighborIndex != currentIndex) {
                isBoundary = true;
                break;
            }
        }
        if (isBoundary)
        break;
    }
    if (isBoundary) {
        outColor = vec4(push.selectionColor.rgb, 1.0);
        return;
    }
    discard;

}