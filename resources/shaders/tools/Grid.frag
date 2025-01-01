#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../DepthUtils.glsl"

layout(push_constant) uniform Push {
    bool overlayObjects;
    float scale;
    int threshold;
    float thickness;
} push;

#define OVERLAY_OBJECTS push.overlayObjects
#define SCALE push.scale
#define THRESHOLD push.threshold
#define THICKNESS push.thickness

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;
layout(set = 1, binding = 0) uniform sampler2D gBufferDepthIdUV;

#define DITHER_MATRIX mat4(\
     0.0, 0.5, 0.125, 0.625, \
     0.75, 0.25, 0.875, 0.375, \
     0.1875, 0.6875, 0.0625, 0.5625, \
     0.9375, 0.4375, 0.8125, 0.312\
)

vec3 p = vec3(0);
bool rayMarch(vec3 ro, vec3 rd, float width) {
    float t = 0.0;
    for (int i = 0; i < 256; i++) {
        p = ro + t * rd;
        float d = p.y;
        if (d < 0.001) return true;
        if (t > THRESHOLD || d < 0.0) break;
        t += max(d, 0.001);
    }
    return false;
}

float getGridLine(float gridScale){
    float scale = gridScale * SCALE;
    float dx = abs(fract(p.x / scale) * scale);
    float dz = abs(fract(p.z / scale) * scale);
    return step(THICKNESS, min(dx, dz));
}

void main() {
    float depthData = getLogDepth(texCoords, gBufferDepthIdUV, globalData.logDepthFC);
    bool hasData = false;
    bool isOverlay = false;
    if (depthData != 1){
        hasData = OVERLAY_OBJECTS;
        isOverlay = true;
        vec3 viewSpacePosition = viewSpacePositionFromDepth(depthData, texCoords, globalData.invProj);
        p = vec3(globalData.invView * vec4(viewSpacePosition, 1.));
    } else {
        vec3 rayDir = createRay(texCoords, globalData.invProj, globalData.invView);
        hasData = rayMarch(globalData.cameraWorldPosition.xyz, rayDir, 1);
    }

    if (hasData){
        float distanceFromCamera = length(globalData.cameraWorldPosition.xyz - p.xyz);
        float alpha = 1;
        if (distanceFromCamera >= THRESHOLD){
            alpha = 0;
        } else {
            float opacity = abs(distanceFromCamera - THRESHOLD) / ((distanceFromCamera + THRESHOLD) / 2.);
            alpha = min(1, opacity);
        }

        if (alpha > 0){
            vec3 baseColor = vec3(0.3);
            vec3 xAxisColor = vec3(1.0, 0.0, 0.0);
            vec3 zAxisColor = vec3(0.0, 0.0, 1.0);

            float isXAxis = step(abs(p.z/ SCALE), isOverlay ? 0 : THICKNESS);
            float isZAxis = step(abs(p.x/ SCALE), isOverlay ? 0 : THICKNESS);

            vec3 axisColor = mix(zAxisColor, xAxisColor, isXAxis);
            vec4 centerLineColor = vec4(axisColor, 1.0) * max(isXAxis, isZAxis);

            vec4 gridColor = mix(vec4(baseColor, 1.0), vec4(0), getGridLine(1));
            gridColor = mix(vec4(baseColor/2., 1.0), gridColor, getGridLine(5));
            finalColor = mix(gridColor, centerLineColor, max(isXAxis, isZAxis));
        }
        finalColor.a = min(alpha, finalColor.a);
        if (finalColor.a == 0){
            discard;
        }
        ivec2 coords = ivec2(mod(gl_FragCoord.xy, 4.0));
        float threshold = DITHER_MATRIX[coords.y][coords.x];
        if (finalColor.a <= threshold) {
            discard;
        }
    } else {
        discard;
    }
}