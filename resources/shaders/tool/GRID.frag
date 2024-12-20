layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 view;
    mat4 proj;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
} globalData;

const vec4 settings = vec4(0, 1, 100, .02);

//uniform sampler2D sceneDepth;
//
//#include "../util/SCENE_DEPTH_UTILS.glsl"
#define OVERLAY_OBJECTS settings.x == 1.
#define SCALE settings.y
#define THREASHOLD settings.z
#define THICKNESS settings.w

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;

vec3 createRay() {
    // Adjust texCoords for Vulkan's coordinate system
    vec2 pxNDS = vec2(texCoords.x, 1.0 - texCoords.y) * 2.0 - 1.0;
    vec3 pointNDS = vec3(pxNDS, -1.0);
    vec4 pointNDSH = vec4(pointNDS, 1.0);

    // Transform from NDC to eye space
    vec4 dirEye = globalData.invProj * pointNDSH;
    dirEye.w = 0.0;

    // Transform from eye space to world space
    vec3 dirWorld = (globalData.invView * dirEye).xyz;
    return normalize(dirWorld);
}

vec3 p = vec3(0);
bool rayMarch(vec3 ro, vec3 rd, float width) {
    float t = 0.0;
    for (int i = 0; i < 256; i++) {
        p = ro + t * rd;
        float d = p.y;
        if (d < 0.001) return true;
        if (t > THREASHOLD) break;
        t += d;
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
    vec3 rayDir = createRay();
    hasData = rayMarch(globalData.cameraWorldPosition.xyz, rayDir, 1);
    //    }

    if (hasData){
        float distanceFromCamera = length(globalData.cameraWorldPosition.xyz - p.xyz);
        float alpha = 1;
        if (distanceFromCamera >= THREASHOLD){
            alpha = 0;
        } else {
            float opacity = abs(distanceFromCamera - THREASHOLD) / ((distanceFromCamera + THREASHOLD) / 2.);
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