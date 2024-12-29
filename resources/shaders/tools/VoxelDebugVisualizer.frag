#include "./GlobalDataBuffer.glsl"
#include "./CreateRay.glsl"
#define DEBUG_VOXELS
#include "./VoxelRaytracing.glsl"

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;

#define RANDOM 0
#define ALBEDO 1
#define NORMAL 2
#define ROUGHNESS 3
#define METALLIC 4

layout(push_constant) uniform Push {
    int voxelDebugFlag;
    bool showRaySearchCount;
    bool showRayTestCount;
} settings;

float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 71.9898, 78.233))) * 43758.5453);
}
vec3 randomColor(float seed) {
    float r = rand(vec3(seed));
    float g = rand(vec3(seed + r));
    return vec3(r, g, rand(vec3(seed + g)));
}

void main() {
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    vec3 colorData = vec3(0);
    Ray ray = Ray(rayOrigin, rayDirection, 1./rayDirection);
    Hit hitData = traceAllTiles(ray, settings.showRaySearchCount, settings.showRayTestCount, colorData);
    if (length(colorData) > 0){
        switch (settings.voxelDebugFlag){
            case ALBEDO:
            finalColor = vec4(randomColor(rand(hitData.hitPosition.xyz)), 1);
            break;
            case NORMAL:
            finalColor = vec4(randomColor(rand(hitData.hitPosition.xyz)), 1);
            break;
            case ROUGHNESS:
            finalColor = vec4(randomColor(rand(hitData.hitPosition.xyz)), 1);
            break;
            case METALLIC:
            finalColor = vec4(randomColor(rand(hitData.hitPosition.xyz)), 1);
            break;
            default :
            finalColor = vec4(randomColor(rand(hitData.hitPosition.xyz)), 1);
            break;
        }
    } else {
        discard;
    }
}