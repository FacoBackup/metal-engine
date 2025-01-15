#include "./GlobalDataBuffer.glsl"
#include "./GBufferUtil.glsl"


layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D gBufferAlbedo;
layout(set = 2, binding = 0) uniform sampler2D gBufferNormal ;
layout(set = 3, binding = 0) uniform sampler2D gBufferPosition;
#define LIGHTS_SET 4
layout(set = 5, binding = 0) uniform sampler2D globalIlluminationSampler;

#include "./Shading.glsl"

#ifdef DEBUG
#include "./DebugFlags.glsl"
#endif

layout(location = 0) out vec4 finalColor;

void main() {
    vec4 worldPos = texture(gBufferPosition, texCoords); // Position + ID
    if (worldPos.a == 0.){
        discard;
    }
    vec4 materialA = texture(gBufferAlbedo, texCoords); // Albedo + (isEmissive ? -1 : 1) * AO
    vec4 materialB = texture(gBufferNormal , texCoords); // Normal + Metallic | Roughness

    vec4 albedoEmissive = materialA;
    ShaderData shaderData;
    shaderData.N = normalize(materialB.rgb);

    vec2 metallicRoughness = decompressRoughnessMetallic(materialB.a);
    shaderData.metallic = metallicRoughness.x;
    shaderData.roughness = max(metallicRoughness.y, .015);
    shaderData.viewSpacePosition = vec3(globalData.viewMatrix * worldPos);
    shaderData.worldSpacePosition = worldPos.rgb;

    vec4 globalIllumination =vec4(0, 0, 0, 1);
    if (globalData.giEnabled){
        globalIllumination = texture(globalIlluminationSampler, texCoords);
    }

    shaderData.ambientOcclusion = abs(materialA.a) * globalIllumination.a;
    shaderData.V = normalize(globalData.cameraWorldPosition - shaderData.worldSpacePosition);
    shaderData.distanceFromCamera = length(shaderData.V);
    shaderData.albedo = albedoEmissive.rgb;
    shaderData.VrN = reflect(-shaderData.V, shaderData.N);
    shaderData.albedoOverPI = shaderData.albedo / PI;
    shaderData.NdotV = clamp(dot(shaderData.N, shaderData.V), 0., 1.);
    shaderData.F0 = vec3(0.04);
    shaderData.F0 = mix(shaderData.F0, shaderData.albedo, shaderData.metallic);
    shaderData.sunDirection = globalData.sunPosition;
    shaderData.sunColor = globalData.sunColor;
    shaderData.lightsQuantity = globalData.lightsQuantity;
    shaderData.enabledSun = globalData.enabledSun;

    #ifdef DEBUG
    if (globalData.debugFlag != LIT){
        bool shouldReturn = true;
        if (globalData.debugFlag == NORMAL){
            finalColor = vec4(shaderData.N, 1);
        } else if (globalData.debugFlag == ROUGHNESS){
            finalColor = vec4(vec3(shaderData.roughness), 1);
        } else if (globalData.debugFlag == METALLIC){
            finalColor = vec4(vec3(shaderData.metallic), 1);
        } else if (globalData.debugFlag == AO){
            finalColor = vec4(vec3(shaderData.ambientOcclusion), 1);
        } else if (globalData.debugFlag == DEPTH){
            finalColor = vec4(vec3(worldPos.a / 100.), 1);
        } else if (globalData.debugFlag == UV){
            finalColor = vec4(texture(gBufferAlbedo, texCoords).rgb, 1);
        } else if (globalData.debugFlag == RANDOM){
            finalColor = vec4(albedoEmissive.rgb, 1);
        } else if (globalData.debugFlag == LIGHTING_ONLY){
            shaderData.albedo = vec3(1, 1, 1);
            shouldReturn = false;
        } else if (globalData.debugFlag == POSITION){
            finalColor = vec4(shaderData.worldSpacePosition, 1);
        } else if (globalData.debugFlag == EMISSIVE){
            finalColor = vec4(albedoEmissive.a < 0 ? vec3(1) : vec3(0), 1);
        } else if (globalData.debugFlag == GI){
            finalColor = vec4(globalIllumination.rgb, 1);
        } else {
            finalColor = vec4(shaderData.albedo, 1);
        }
        if (shouldReturn) return;
    }
    #endif

    if (materialA.a < 0) { // EMISSION
        finalColor = vec4(albedoEmissive.rgb, 1.);
        return;
    }
    finalColor = vec4(physicallyBasedShadePixel(shaderData) + shaderData.albedoOverPI * globalIllumination.rgb * globalData.giStrength, 1.);
}
