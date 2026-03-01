#include "../GlobalDataBuffer.glsl"

layout (set = 0, binding = 1, r32f) uniform readonly image2D gBufferPositionIndex;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Push {
    mat4 model;
    vec4 selectionColor; // xyz: color, w: thickness
    uint renderIndex;
} push;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(globalData.outputRes);
    uint currentIndex = uint(abs(imageLoad(gBufferPositionIndex, ivec2(gl_FragCoord.xy)).r));

    int thickness = int(push.selectionColor.a);
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

            uint neighborIndex = uint(abs(imageLoad(gBufferPositionIndex, neighborPixel).r));

            if (neighborIndex != push.renderIndex + 1) {
                isBoundary = true;
                break;
            }
        }
        if (isBoundary)
        break;
    }
    bool isDiff = currentIndex != (push.renderIndex + 1);
    if (isBoundary && !isDiff) {
        outColor = vec4(push.selectionColor.rgb, 1.0);
    } else  {
        int dotSpacing = 10;
        int dotRadius = 2;

        vec2 gridIndex = floor(gl_FragCoord.xy / float(dotSpacing));
        vec2 gridCenter = (gridIndex + 0.5) * float(dotSpacing);

        float dist = distance(gl_FragCoord.xy, gridCenter);
        if (dist >= dotRadius) {
            discard;
        }
        outColor = vec4(push.selectionColor.rgb, 1.0);

        if (isDiff) {
            outColor.a = .5;
        }
    }

}