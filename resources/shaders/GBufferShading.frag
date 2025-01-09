#include "./GlobalDataBuffer.glsl"


layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D gBufferMaterialA;
layout(set = 2, binding = 0) uniform sampler2D gBufferMaterialB;
layout(set = 3, binding = 0) uniform sampler2D gBufferMaterialC;
layout(set = 4, binding = 0) uniform sampler2D gBufferMaterialD;
#define LIGHTS_SET 5
layout(set = 6, binding = 0) uniform sampler2D globalIlluminationSampler;

#include "./Shading.glsl"

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
    vec4 worldPos = texture(gBufferMaterialD, texCoords);
    if (worldPos.a != 1.){
        discard;
    }
    vec4 materialA = texture(gBufferMaterialA, texCoords);
    vec4 materialB = texture(gBufferMaterialB, texCoords);

    vec4 albedoEmissive = materialA;
    ShaderData shaderData;
    shaderData.N = normalize(materialB.rgb);
    shaderData.roughness = max(materialB.a, .015);
    shaderData.metallic = materialC.b;
    shaderData.viewSpacePosition = vec3(globalData.viewMatrix * worldPos);
    shaderData.worldSpacePosition = worldPos.rgb;

    vec4 globalIllumination = globalData.giEnabled ? texture(globalIlluminationSampler, texCoords) :  vec4(0, 0, 0, 1);
    shaderData.ambientOcclusion = materialC.g * globalIllumination.a;
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
            finalColor = vec4(worldPos.rgb, 1);
        } else if (globalData.debugFlag == UV){
            finalColor = vec4(texture(gBufferMaterialC, texCoords).zw, 0, 1);
        } else if (globalData.debugFlag == RANDOM){
            finalColor = vec4(randomColor(int(texture(gBufferMaterialC, texCoords).r)), 1);
        } else if (globalData.debugFlag == LIGHTING_ONLY){
            shaderData.albedo = vec3(1, 1, 1);
            shouldReturn = false;
        } else if (globalData.debugFlag == POSITION){
            float voxelSize = float(globalData.giTileSubdivision);
            finalColor = vec4(normalize(round(shaderData.worldSpacePosition * voxelSize) / voxelSize), 1);
        } else if (globalData.debugFlag == EMISSIVE){
            finalColor = vec4(albedoEmissive.a > 0 ? vec3(1) : vec3(0), 1);
        } else if (globalData.debugFlag == GI){
            finalColor = vec4(globalIllumination.rgb, 1);
        } else {
            finalColor = vec4(shaderData.albedo, 1);
        }
        if(shouldReturn) return;
    }
    #endif

    if (albedoEmissive.a > 0) { // EMISSION
        finalColor = vec4(albedoEmissive.rgb, 1.);
        return;
    }
    finalColor = vec4(physicallyBasedShadePixel(shaderData) + shaderData.albedoOverPI * globalIllumination.rgb * globalData.giStrength, 1.);
}
