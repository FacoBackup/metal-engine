#include "./GlobalDataBuffer.glsl"
#include "./CreateRay.glsl"
#include "./VoxelRaytracing.glsl"

#define LIGHTS_SET 2
#define LIGHTS_BINDING 0
#include "./LightsBuffer.glsl"

layout(push_constant) uniform Push {
    float bias;
    float biasHit;
    float shadowsBaseColor;
} push;

layout(location = 0) in vec2 texCoords;
layout(location = 0) out float finalColor;

void main() {
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    Ray ray = Ray(rayOrigin, rayDirection, 1./rayDirection);
    Hit hitData = traceAllTiles(ray);
    if (hitData.anyHit){
        if (globalData.lightsQuantity > 0){
            Light l = lightsBuffer.lights[0];
            vec3 firstHitPos = hitData.hitPosition * push.bias;
            rayDirection = normalize(l.position - firstHitPos);
            ray = Ray(firstHitPos, rayDirection, 1./rayDirection);
            hitData = traceAllTiles(ray);
            float lightDistance = length(l.position - firstHitPos);
            float hitDistance = length(hitData.hitPosition - firstHitPos);
            if (hitData.anyHit && hitDistance < lightDistance && hitDistance > push.biasHit) {
                finalColor = 1.0 - push.shadowsBaseColor / lightDistance;
            } else {
                finalColor = 1;
            }
        }
    } else{
        finalColor = 1;
    }
}