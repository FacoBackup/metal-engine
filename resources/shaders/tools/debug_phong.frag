#include "../GlobalDataBuffer.glsl"
#define MESH_METADATA_SET 1
#include "../MeshMetadata.glsl"
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

// Debug shading modes (aligned with ShadingMode.h)
const uint DEBUG_MODE_LIT           = 0;
const uint DEBUG_MODE_ALBEDO        = 1;
const uint DEBUG_MODE_NORMAL        = 2;
const uint DEBUG_MODE_ROUGHNESS     = 3;
const uint DEBUG_MODE_METALLIC      = 4;
const uint DEBUG_MODE_RANDOM        = 5;
const uint DEBUG_MODE_DEPTH         = 6;
const uint DEBUG_MODE_UV            = 7;
const uint DEBUG_MODE_POSITION      = 8;
const uint DEBUG_MODE_LIGHTING_ONLY = 9;
const uint DEBUG_MODE_EMISSIVE      = 10;

vec3 randomColor(int seed) {
    float hash = fract(sin(float(seed)) * 43758.5453);
    float r = fract(hash * 13.756);
    float g = fract(hash * 15.734);
    float b = fract(hash * 17.652);
    return vec3(r, g, b);
}

void main() {
    MeshMetadata metadata = meshMetadataBuffer.items[push.renderIndex];
    vec3 baseColor;
    float roughness;
    float metallic;

    if (metadata.albedoTexture != 0u) {
        baseColor = texture(textureArray[nonuniformEXT(metadata.albedoTexture)], inUV).rgb;
    } else {
        baseColor = getDebugGrid(inWorldPos).baseColor;
    }
    if (metadata.roughnessTexture != 0u) {
        roughness = texture(textureArray[nonuniformEXT(metadata.roughnessTexture)], inUV).r;
    } else {
        roughness = getDebugGrid(inWorldPos).roughness;
    }
    if (metadata.metallicTexture != 0u) {
        metallic = texture(textureArray[nonuniformEXT(metadata.metallicTexture)], inUV).r;
    } else {
        metallic = getDebugGrid(inWorldPos).metallic;
    }

    vec3 finalColor;

    if (globalData.debugFlag == DEBUG_MODE_NORMAL) {
        finalColor = normalize(inNormal);
    } else if (globalData.debugFlag == DEBUG_MODE_ROUGHNESS) {
        finalColor = vec3(roughness);
    } else if (globalData.debugFlag == DEBUG_MODE_METALLIC) {
        finalColor = vec3(metallic);
    } else if (globalData.debugFlag == DEBUG_MODE_DEPTH) {
        finalColor = vec3(distance(inWorldPos, globalData.cameraWorldPosition) / 25.0);
    } else if (globalData.debugFlag == DEBUG_MODE_UV) {
        finalColor = vec3(inUV, 0);
    } else if (globalData.debugFlag == DEBUG_MODE_RANDOM) {
        finalColor = randomColor(int(push.renderIndex + 1));
    } else if (globalData.debugFlag == DEBUG_MODE_POSITION) {
        finalColor = normalize(inWorldPos);
    } else if (globalData.debugFlag == DEBUG_MODE_EMISSIVE) {
        finalColor = metadata.isEmissive == 1u ? vec3(1) : vec3(0);
    } else {
        finalColor = baseColor;
    }

    outFragColor = vec4(finalColor, 1.0);
}
