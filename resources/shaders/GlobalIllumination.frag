#include "./GlobalDataBuffer.glsl"
#include "./CreateRay.glsl"
#include "./VoxelRaytracing.glsl"
#include "./Dithering.glsl"

#define LIGHTS_SET 2
#define LIGHTS_BINDING 0
#include "./LightsBuffer.glsl"

layout(set = 3, binding = 0) uniform sampler2D gBufferNormal;

layout(push_constant) uniform Push {
    float biasHit;
    float shadowsBaseColor;
    float ditheringIntensity;
    uint giBounces;
    uint giSamplesPerPixel;
} push;

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;


float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 71.9898, 78.233))) * 43758.5453);
}
vec3 sampleHemisphere(vec3 normal, uint i) {
    float xi1 = fract(sin(float(i) * 12.9898) * 43758.5453);
    float xi2 = fract(sin(float(i) * 78.233) * 43758.5453);
    float theta = acos(sqrt(1.0 - xi1));
    float phi = 2.0 * 3.141592 * xi2;
    vec3 tangent = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    vec3 bitangent = cross(normal, tangent);
    vec3 sampleDir = tangent * sin(theta) * cos(phi) +
    bitangent * sin(theta) * sin(phi) +
    normal * cos(theta);

    return normalize(sampleDir);
}

vec3 computeDiffuseIndirectLight(Ray initialRay, uint maxBounces, float bias) {
    vec3 indirectLight = vec3(0.0);// Accumulated indirect light
    vec3 throughput = vec3(1.0);// Tracks light contribution across bounces
    Ray ray = initialRay;

    for (uint bounce = 0; bounce < maxBounces; ++bounce) {
        Hit hitData = traceAllTiles(ray);

        if (!hitData.anyHit) {
            break;
        }
        vec3 hitPos = hitData.hitPosition;
        VoxelMaterialData matData = unpackVoxel(hitData);
        vec3 hitNormal = matData.normal;
        vec3 albedo = matData.albedo;
        vec3 randomDir = sampleHemisphere(hitNormal, bounce);
        float NdotL = max(dot(hitNormal, randomDir), 0.0);
        vec3 bounceLight = albedo * NdotL;
        indirectLight += throughput * bounceLight;
        throughput *= albedo;
        ray.o = hitPos + hitNormal * bias;
        ray.d = randomDir;
    }

    return indirectLight;
}

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
    if (push.ditheringIntensity > 0 && isDitherDiscard(1. - push.ditheringIntensity)){
        discard;
    }
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    Ray ray = Ray(rayOrigin, rayDirection, 1./rayDirection);
    Hit hitData = traceAllTiles(ray);
    finalColor = vec4(0, 0, 0, 1);
    if (hitData.anyHit){
        discard;
    }

    vec3 firstHitPos = hitData.hitPosition + texture(gBufferNormal, texCoords).rgb;
    // TODO - ADD CONTRIBUTION TO EVERY LIGHT WITH SHADOWS ENABLED
    if (globalData.lightsQuantity > 0){
        Light l = lightsBuffer.lights[0];
        if (length(l.position - hitData.hitPosition) < l.outerRadius) {
            finalColor.a = testLight(l.position, hitData, firstHitPos);
        }
    }
    if (globalData.enabledSun){
        finalColor.a *= testLight(normalize(globalData.sunPosition) * 20, hitData, firstHitPos);
    }
    if(push.giBounces > 0) {
        VoxelMaterialData matData = unpackVoxel(hitData);
        for (uint i = 0; i < push.giSamplesPerPixel; i++) {
            vec3 randomDir = sampleHemisphere(matData.normal, i);
            ray = Ray(firstHitPos, randomDir, 1.0 / randomDir);

//            Hit aoHitData = traceAllTiles(ray);
//            if (aoHitData.anyHit) {
//                float distance = length(aoHitData.hitPosition - firstHitPos);
//                finalColor.a *= exp(-distance * AO_FALLOFF);
                finalColor.rgb += computeDiffuseIndirectLight(ray, push.giBounces, push.biasHit);
//            }
        }
    }
}