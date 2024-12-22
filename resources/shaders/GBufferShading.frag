#include "./GlobalData.glsl"
#include "./DepthUtils.glsl"
#include "./Shading.glsl"

#define LIT 0
#define ALBEDO 1
#define NORMAL 2
#define ROUGHNESS 3
#define METALLIC 4
#define AO 5
#define RANDOM 6
#define DEPTH 7
#define UV 8

layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D gBufferAlbedoEmissive;
layout(set = 2, binding = 0) uniform sampler2D gBufferRoughnessMetallicAO;
layout(set = 3, binding = 0) uniform sampler2D gBufferNormal;
layout(set = 4, binding = 0) uniform sampler2D gBufferDepthIdUV;
layout(set = 5, binding = 0) uniform sampler2D brdfSampler;

#ifdef DEBUG
layout(push_constant) uniform Push {
    int mode;
} push;
#endif

layout(location = 0) out vec4 finalColor;

void main() {
    float depthData = getLogDepth(texCoords, gBufferDepthIdUV, globalData.logDepthFC);
    if (depthData == 1.){
        discard;
    }

    #ifdef DEBUG
    if (push.mode != LIT){
        if (push.mode == ALBEDO){
            finalColor = vec4(texture(gBufferAlbedoEmissive, texCoords).rgb, 1);
        } else if (push.mode == NORMAL){
            finalColor = vec4(texture(gBufferNormal, texCoords).rgb, 1);
        } else if (push.mode == ROUGHNESS){
            finalColor = vec4(vec3(texture(gBufferRoughnessMetallicAO, texCoords).r), 1);
        } else if (push.mode == METALLIC){
            finalColor = vec4(vec3(texture(gBufferRoughnessMetallicAO, texCoords).g), 1);
        } else if (push.mode == AO){
            finalColor = vec4(vec3(texture(gBufferRoughnessMetallicAO, texCoords).b), 1);
        } else if (push.mode == DEPTH){
            finalColor = vec4(vec3(texture(gBufferDepthIdUV, texCoords).r), 1);
        } else if (push.mode == UV){
            finalColor = vec4(texture(gBufferDepthIdUV, texCoords).zw, 0, 1);
        } else {
            finalColor = vec4(1, 0, 1, 1);
        }
        return;
    }
    #endif

    vec4 albedoEmissive = texture(gBufferAlbedoEmissive, texCoords);
    if (albedoEmissive.a > 0) { // EMISSION
        finalColor = vec4(albedoEmissive.rgb, 1.);
        return;
    }
    ShaderData shaderData;
    shaderData.N = normalize(texture(gBufferNormal, texCoords).rgb);
    vec3 valueRMAOSampler = texture(gBufferRoughnessMetallicAO, texCoords).rgb;
    shaderData.roughness = valueRMAOSampler.r;
    shaderData.metallic = valueRMAOSampler.g;
    shaderData.ambientOcclusion = valueRMAOSampler.b;
    shaderData.viewSpacePosition = viewSpacePositionFromDepth(depthData, texCoords, globalData.invProj);
    shaderData.worldSpacePosition = vec3(globalData.invView * vec4(shaderData.viewSpacePosition, 1.));
    shaderData.V = normalize(globalData.cameraWorldPosition.xyz - shaderData.worldSpacePosition);
    shaderData.distanceFromCamera = length(shaderData.V);
    shaderData.albedo = albedoEmissive.rgb;
    shaderData.VrN = reflect(-shaderData.V, shaderData.N);
    shaderData.albedoOverPI = shaderData.albedo / PI;
    shaderData.NdotV = clamp(dot(shaderData.N, shaderData.V), 0., 1.);
    shaderData.brdf = texture(brdfSampler, vec2(shaderData.NdotV, shaderData.roughness)).rg;
    shaderData.F0 = vec3(0.04);
    shaderData.F0 = mix(shaderData.F0, shaderData.albedo, shaderData.metallic);
    shaderData.sunDirection = globalData.sunPosition;
    shaderData.sunColor = globalData.sunColor;

    finalColor = vec4(physicallyBasedShadePixel(shaderData), 1.);
}
