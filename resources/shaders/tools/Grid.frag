#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../Dithering.glsl"

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
layout(set = 0, binding = 1, rg32f) uniform readonly image2D gBufferRenderIndexDepth;

vec3 p = vec3(0);

bool rayMarch(vec3 ro, vec3 rd, float width) {
    if (abs(rd.y) < 1e-6) return false;
    float t = -ro.y / rd.y;
    if (t < 0.0) return false;
    if (t > float(THRESHOLD)) return false;
    p = ro + t * rd;
    return true;
}

float getGridLine(float gridScale){
    float scale = gridScale * SCALE;
    float dx = abs(fract(p.x / scale) * scale);
    float dz = abs(fract(p.z / scale) * scale);
    return step(THICKNESS, min(dx, dz));
}

void main() {
    vec2 renderIndexDepth = imageLoad(gBufferRenderIndexDepth, ivec2(gl_FragCoord.xy)).rg;
    float renderIndex = renderIndexDepth.r;
    float depth = renderIndexDepth.g;
    bool hasData = false;
    bool isOverlay = false;
    if (renderIndex != 0){
        hasData = OVERLAY_OBJECTS;
        isOverlay = true;

        // Reconstruct position from depth
        vec4 clipPos;
        clipPos.xy = texCoords * 2.0 - 1.0;
        clipPos.z = depth;
        clipPos.w = 1.0;
        vec4 worldPos = globalData.invProjView * clipPos;
        p = worldPos.xyz / worldPos.w;
    } else {
        vec3 rayOrigin;
        vec3 rayDir = createRay(texCoords, globalData.invProj, globalData.invView, rayOrigin);
        hasData = rayMarch(rayOrigin, rayDir, 1);
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
        if (isDitherDiscard(finalColor.a)) {
            discard;
        }
    } else {
        discard;
    }
}