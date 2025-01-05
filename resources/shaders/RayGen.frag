#include "./GlobalDataBuffer.glsl"
#include "./CreateRay.glsl"
#include "./VoxelRaytracing.glsl"

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 voxelPosition;
layout(location = 1) out vec4 hitPosition;

void main() {
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    Ray ray = Ray(rayOrigin, rayDirection, 1./rayDirection, rayOrigin, false);
    Hit hitData = traceAllTiles(ray);
    if(hitData.anyHit) {
        voxelPosition = vec4(hitData.voxelPosition.xyz, 1);
        hitPosition = vec4(hitData.hitPosition.xyz, 1);
    }
}