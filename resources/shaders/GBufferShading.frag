#include "./GlobalDataBuffer.glsl"
#include "./DepthUtils.glsl"


#define LIGHTS_SET 7
#define LIGHTS_BINDING 0

#include "./Shading.glsl"
#include "./DebugFlags.glsl"


layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D gBufferAlbedoEmissive;
layout(set = 2, binding = 0) uniform sampler2D gBufferRoughnessMetallicAO;
layout(set = 3, binding = 0) uniform sampler2D gBufferNormal;
layout(set = 4, binding = 0) uniform sampler2D gBufferDepthIdUV;
layout(set = 5, binding = 0) uniform sampler2D brdfSampler;
layout(set = 6, binding = 0) uniform sampler2D giSampler;

#ifdef DEBUG
layout(push_constant) uniform Push {
    int mode;
} push;

vec3 randomColor(int seed) {
    float hash = fract(sin(float(seed)) * 43758.5453);

    float r = fract(hash * 13.756);
    float g = fract(hash * 15.734);
    float b = fract(hash * 17.652);

    return vec3(r, g, b);
}
#endif

layout(location = 0) out vec4 finalColor;

void main() {
    float depthData = getLogDepth(texCoords, gBufferDepthIdUV, globalData.logDepthFC);
    if (depthData == 1.){
        discard;
    }

    vec4 albedoEmissive = texture(gBufferAlbedoEmissive, texCoords);
    vec4 globalIllumination = texture(giSampler, texCoords);
    ShaderData shaderData;
    shaderData.N = normalize(texture(gBufferNormal, texCoords).rgb);
    vec3 valueRMAOSampler = texture(gBufferRoughnessMetallicAO, texCoords).rgb;
    shaderData.roughness = valueRMAOSampler.r;
    shaderData.metallic = valueRMAOSampler.g;
    shaderData.ambientOcclusion = valueRMAOSampler.b * globalIllumination.a;
    shaderData.viewSpacePosition = viewSpacePositionFromDepth(depthData, texCoords, globalData.invProj);
    shaderData.worldSpacePosition = vec3(globalData.invView * vec4(shaderData.viewSpacePosition, 1));
    shaderData.V = normalize(globalData.cameraWorldPosition - shaderData.worldSpacePosition);
    shaderData.distanceFromCamera = length(shaderData.V);
    shaderData.albedo = albedoEmissive.rgb;
    shaderData.VrN = reflect(-shaderData.V, shaderData.N);
    shaderData.albedoOverPI = shaderData.albedo / PI;
    shaderData.NdotV = clamp(dot(shaderData.N, shaderData.V), 0., 1.);
    shaderData.brdf = texture(brdfSampler, vec2(shaderData.NdotV, 1-shaderData.roughness)).rg;
    shaderData.F0 = vec3(0.04);
    shaderData.F0 = mix(shaderData.F0, shaderData.albedo, shaderData.metallic);
    shaderData.sunDirection = globalData.sunPosition;
    shaderData.sunColor = globalData.sunColor;
    shaderData.lightsQuantity = globalData.lightsQuantity;
    shaderData.enabledSun = globalData.enabledSun;

    #ifdef DEBUG
    if (push.mode != LIT){
        if (push.mode == NORMAL){
            finalColor = vec4(shaderData.N, 1);
        } else if (push.mode == ROUGHNESS){
            finalColor = vec4(vec3(shaderData.roughness), 1);
        } else if (push.mode == METALLIC){
            finalColor = vec4(vec3(shaderData.metallic), 1);
        } else if (push.mode == AO){
            finalColor = vec4(vec3(shaderData.ambientOcclusion), 1);
        } else if (push.mode == DEPTH){
            finalColor = vec4(vec3(depthData), 1);
        } else if (push.mode == UV){
            finalColor = vec4(texture(gBufferDepthIdUV, texCoords).zw, 0, 1);
        } else if (push.mode == RANDOM){
            finalColor = vec4(randomColor(int(texture(gBufferDepthIdUV, texCoords).g)), 1);
        } else if (push.mode == BRDF){
            finalColor = vec4(shaderData.brdf, 0, 1);
        } else if (push.mode == POSITION){
            finalColor = vec4(normalize(shaderData.worldSpacePosition), 1);
        } else if (push.mode == EMISSIVE){
            finalColor = vec4(albedoEmissive.a > 0 ? vec3(1) : vec3(0), 1);
        } else if (push.mode == GI){
            finalColor = vec4(globalIllumination.rgb, 1);
        } else {
            finalColor = vec4(shaderData.albedo, 1);
        }
        return;
    }
    #endif

    if (albedoEmissive.a > 0) { // EMISSION
        finalColor = vec4(albedoEmissive.rgb, 1.);
        return;
    }
    finalColor = vec4(physicallyBasedShadePixel(shaderData) + globalIllumination.rgb, 1.);
}
