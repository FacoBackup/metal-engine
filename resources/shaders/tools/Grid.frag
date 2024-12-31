#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"

const vec4 settings = vec4(0, 1, 100, .02);

#define OVERLAY_OBJECTS settings.x == 1.
#define SCALE settings.y
#define THRESHOLD settings.z
#define THICKNESS settings.w

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;


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
    //    float depthData = getLogDepth(texCoords);

    bool hasData = false;
    bool isOverlay = false;
    //    if (depthData != 1){
    //        hasData = OVERLAY_OBJECTS;
    //        isOverlay = true;
    //        vec3 viewSpacePosition = viewSpacePositionFromDepth(depthData, texCoords);
    //        p = vec3(invViewMatrix * vec4(viewSpacePosition, 1.));
    //    } else {
    vec3 rayDir = createRay(texCoords, globalData.invProj, globalData.invView);
    hasData = rayMarch(globalData.cameraWorldPosition.xyz, rayDir, 1);
    //    }

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
    } else {
        discard;
    }
}