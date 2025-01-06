#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#define DEBUG_VOXELS
#include "../RayTracer.glsl"
#include "../DebugFlags.glsl"

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;

layout(push_constant) uniform Push {
    int voxelDebugFlag;
    bool showRaySearchCount;
    bool showRayTestCount;
    uint searchCountDivisor;
} settings;

vec3 randomColor(float seed) {
    float r = rand(vec3(seed));
    float g = rand(vec3(seed + r));
    return vec3(r, g, rand(vec3(seed + g)));
}

void main() {
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    ivec2 colorData = ivec2(0);
    Ray ray = Ray(rayOrigin, rayDirection, 1./rayDirection);
    Hit hitData = traceAllTiles(ray, settings.showRaySearchCount, settings.showRayTestCount, colorData);
    VoxelMaterialData matData = unpackVoxel(hitData);
    switch (settings.voxelDebugFlag){
        case ALBEDO:
        finalColor = vec4(matData.albedo, 1);
        break;
        case NORMAL:
        finalColor = vec4(matData.normal, 1);
        break;
        case ROUGHNESS:
        finalColor = vec4(vec3(matData.roughness), 1);
        break;
        case METALLIC:
        finalColor = vec4(vec3(matData.metallic), 1);
        break;
        case EMISSIVE:
        finalColor = vec4(vec3(matData.isEmissive ? 1 : 0), 1);
        break;
        case RANDOM:
        finalColor = vec4(randomColor(rand(hitData.voxelPosition.xyz)), 1);
        break;
        case GI:
        //        float GRID_SIZE = getLevelOfDetailTile(length(globalData.cameraWorldPosition - hitData.voxelPosition));
        finalColor = vec4(hashWorldSpaceCoord(hitData.voxelPosition.xyz), 0, 1);
        break;
        default :
        finalColor = vec4(normalize(hitData.voxelPosition.xyz), 1);
        break;
    }
    if (length(finalColor.rgb) == 0){
        finalColor.rg = colorData/float(settings.searchCountDivisor);
        finalColor.a = 1;
    }
}