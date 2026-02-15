#include "../GlobalDataBuffer.glsl"

layout (set = 1, binding = 0) uniform sampler2D gBufferPositionIndex;

layout (location = 0) flat in uint renderIndex;
layout (location = 0) out vec4 outColor;

#include "../GBufferGenPushConstant.glsl"

void main() {

    vec2 uv = gl_FragCoord.xy / vec2(globalData.outputRes);

    uint currentIndex = uint(abs(texture(gBufferPositionIndex, uv).a));
    if (currentIndex != renderIndex)
    discard;

    int thickness = int(push.albedoEmissive.a);
    bool isBoundary = false;

    ivec2 pixel = ivec2(gl_FragCoord.xy);

    for (int y = -thickness; y <= thickness; ++y) {
        for (int x = -thickness; x <= thickness; ++x) {

            if (x == 0 && y == 0)
            continue;

            ivec2 neighborPixel = pixel + ivec2(x, y);

            if (neighborPixel.x < 0 || neighborPixel.x >= int(globalData.outputRes.x) ||
            neighborPixel.y < 0 || neighborPixel.y >= int(globalData.outputRes.y)) {
                isBoundary = true;
                break;
            }

            vec2 neighborUv = vec2(neighborPixel) / vec2(globalData.outputRes);
            uint neighborIndex = uint(abs(texture(gBufferPositionIndex, neighborUv).a));

            if (neighborIndex != renderIndex) {
                isBoundary = true;
                break;
            }
        }
        if (isBoundary)
        break;
    }

    if (isBoundary) {
        outColor = vec4(push.albedoEmissive.rgb, 1.0);
    } else {
        int dotSpacing = 10;
        int dotRadius = 2;

        vec2 gridIndex = floor(gl_FragCoord.xy / float(dotSpacing));
        vec2 gridCenter = (gridIndex + 0.5) * float(dotSpacing);

        float dist = distance(gl_FragCoord.xy, gridCenter);
        if (dist >= dotRadius) {
            discard;
        }
        outColor = vec4(push.albedoEmissive.rgb, 1.0);
    }
}