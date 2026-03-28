#include "../GlobalDataBuffer.glsl"
#define MATERIAL_DATA_SET 1
#include "../MaterialData.glsl"
#include "../util/GridUtil.glsl"

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

layout (set = 0, binding = 2) uniform sampler2D textureArray[];

layout (push_constant) uniform Push {
    mat4 model;
    uint renderIndex;
} push;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 outFragColor;


vec3 randomColor(int seed) {
    float hash = fract(sin(float(seed)) * 43758.5453);
    float r = fract(hash * 13.756);
    float g = fract(hash * 15.734);
    float b = fract(hash * 17.652);
    return vec3(r, g, b);
}

void main() {
    MaterialData material = materialBuffer.items[push.renderIndex];
    vec3 baseColor;
    float roughness;
    float metallic;

    if (material.albedoTexture != 0u) {
        baseColor = texture(textureArray[nonuniformEXT(material.albedoTexture)], inUV).rgb;
    } else {
        baseColor = getDebugGrid(inWorldPos).baseColor;
    }
    if (material.roughnessTexture != 0u) {
        roughness = texture(textureArray[nonuniformEXT(material.roughnessTexture)], inUV).r;
    } else {
        roughness = getDebugGrid(inWorldPos).roughness;
    }
    if (material.metallicTexture != 0u) {
        metallic = texture(textureArray[nonuniformEXT(material.metallicTexture)], inUV).r;
    } else {
        metallic = getDebugGrid(inWorldPos).metallic;
    }

    vec3 finalColor;

    if (globalData.debugFlag == NORMAL) {
        finalColor = normalize(inNormal);
    } else if (globalData.debugFlag == ROUGHNESS) {
        finalColor = vec3(roughness);
    } else if (globalData.debugFlag == METALLIC) {
        finalColor = vec3(metallic);
    } else if (globalData.debugFlag == DEPTH) {
        finalColor = vec3(distance(inWorldPos, globalData.cameraWorldPosition) / 25.0);
    } else if (globalData.debugFlag == UV) {
        finalColor = vec3(inUV, 0);
    } else if (globalData.debugFlag == RANDOM) {
        finalColor = randomColor(int(push.renderIndex + 1));
    } else if (globalData.debugFlag == POSITION) {
        finalColor = normalize(inWorldPos);
    } else if (globalData.debugFlag == EMISSIVE) {
        finalColor = material.isEmissive == 1u ? vec3(1) : vec3(0);
    } else {
        finalColor = baseColor;
    }

    outFragColor = vec4(finalColor, 1.0);
}
