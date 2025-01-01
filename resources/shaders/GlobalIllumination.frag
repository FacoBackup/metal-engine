#include "./GlobalDataBuffer.glsl"
#include "./CreateRay.glsl"
#include "./VoxelRaytracing.glsl"

#define LIGHTS_SET 2
#define LIGHTS_BINDING 0
#include "./LightsBuffer.glsl"

layout(set = 3, binding = 0) uniform sampler2D gBufferNormal;

layout(push_constant) uniform Push {
    float biasHit;
    float shadowsBaseColor;
} push;

layout(location = 0) in vec2 texCoords;
layout(location = 0) out float finalColor;

float testLight(vec3 lightPosition, Hit hitData, in vec3 firstHitPos){
    vec3 rayDirection = normalize(lightPosition - firstHitPos);
    Ray ray = Ray(firstHitPos, rayDirection, 1./rayDirection);
    hitData = traceAllTiles(ray);
    float lightDistance = length(lightPosition - firstHitPos);
    float hitDistance = length(hitData.hitPosition - firstHitPos);
    if (hitData.anyHit && hitDistance < lightDistance && hitDistance > push.biasHit) {
        return 1.0 - push.shadowsBaseColor / lightDistance;
    }
    return 1;
}

void main() {
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    Ray ray = Ray(rayOrigin, rayDirection, 1./rayDirection);
    Hit hitData = traceAllTiles(ray);
    if (hitData.anyHit){
        finalColor = 1;
        vec3 firstHitPos = hitData.hitPosition + texture(gBufferNormal, texCoords).rgb;
        if (globalData.lightsQuantity > 0){
            Light l = lightsBuffer.lights[0];
            finalColor = testLight(l.position, hitData, firstHitPos);
        }
        if(globalData.enabledSun){
            finalColor *= testLight(normalize(globalData.sunPosition) * 20, hitData, firstHitPos);
        }
    } else{
        finalColor = 1;
    }
}