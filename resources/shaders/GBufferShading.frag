#include "./GlobalDataBuffer.glsl"
#include "./DepthUtils.glsl"


#define LIGHTS_SET 6
#define LIGHTS_BINDING 0

#include "./Shading.glsl"

layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D gBufferMaterialA;
layout(set = 2, binding = 0) uniform sampler2D gBufferMaterialB;
layout(set = 3, binding = 0) uniform sampler2D gBufferMaterialC;
layout(set = 4, binding = 0) uniform sampler2D brdfSampler;
layout(set = 5, binding = 0) uniform sampler2D voxelPositionSampler;
layout(set = 7, binding = 0) uniform sampler2D giSampler;

#ifdef DEBUG
vec3 randomColor(int seed) {
    float hash = fract(sin(float(seed)) * 43758.5453);

    float r = fract(hash * 13.756);
    float g = fract(hash * 15.734);
    float b = fract(hash * 17.652);

    return vec3(r, g, b);
}

#include "./DebugFlags.glsl"

#endif

layout(location = 0) out vec4 finalColor;

void main() {
    vec4 materialC = texture(gBufferMaterialC, texCoords);
    float depthData = getLogDepthFromData(materialC.r, globalData.logDepthFC);
    if (depthData == 1.){
        discard;
    }
    vec4 materialA = texture(gBufferMaterialA, texCoords);
    vec4 materialB = texture(gBufferMaterialB, texCoords);

    vec4 albedoEmissive = materialA;
    ShaderData shaderData;
    shaderData.N = normalize(materialB.rgb);
    shaderData.roughness = materialC.b;
    shaderData.metallic = materialC.a;
    shaderData.viewSpacePosition = viewSpacePositionFromDepth(depthData, texCoords, globalData.invProj);
    shaderData.worldSpacePosition = vec3(globalData.invView * vec4(shaderData.viewSpacePosition, 1));
    vec4 globalIllumination = texture(giSampler, hashWorldSpaceCoord(texture(voxelPositionSampler, texCoords).rgb));
    shaderData.ambientOcclusion = materialB.a * globalIllumination.a;
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
    if (globalData.debugFlag != LIT){
        if (globalData.debugFlag == NORMAL){
            finalColor = vec4(shaderData.N, 1);
        } else if (globalData.debugFlag == ROUGHNESS){
            finalColor = vec4(vec3(shaderData.roughness), 1);
        } else if (globalData.debugFlag == METALLIC){
            finalColor = vec4(vec3(shaderData.metallic), 1);
        } else if (globalData.debugFlag == AO){
            finalColor = vec4(vec3(shaderData.ambientOcclusion), 1);
        } else if (globalData.debugFlag == DEPTH){
            finalColor = vec4(vec3(depthData), 1);
        } else if (globalData.debugFlag == UV){
            finalColor = vec4(texture(gBufferMaterialC, texCoords).zw, 0, 1);
        } else if (globalData.debugFlag == RANDOM){
            finalColor = vec4(randomColor(int(texture(gBufferMaterialC, texCoords).g)), 1);
        } else if (globalData.debugFlag == BRDF){
            finalColor = vec4(shaderData.brdf, 0, 1);
        } else if (globalData.debugFlag == POSITION){
            finalColor = vec4(normalize(shaderData.worldSpacePosition), 1);
        } else if (globalData.debugFlag == EMISSIVE){
            finalColor = vec4(albedoEmissive.a > 0 ? vec3(1) : vec3(0), 1);
        } else if (globalData.debugFlag == GI){
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
