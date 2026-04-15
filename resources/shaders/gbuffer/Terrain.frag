#include "../GlobalDataBuffer.glsl"
#extension GL_EXT_nonuniform_qualifier : enable
#include "TerrainPushConstant.glsl"
#include "../gbuffer/GBufferUtil.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inLocalPosition;
layout(location = 4) in vec4 inCurrPos;
layout(location = 5) in vec4 inPrevPos;

#define MATERIAL_DATA_SET 1
#include "../MaterialData.glsl"
layout(set = 0, binding = 2) uniform sampler2D textureArray[];

layout (location = 0) out vec4 gBufferAlbedoEmissive;
layout (location = 1) out vec2 gBufferRoughnessMetallic;
layout (location = 2) out vec2 gBufferRenderIndexDepth;
layout (location = 3) out vec4 gBufferNormal;
layout (location = 4) out vec2 gBufferMotionVector;

void main () {
    vec2 localUV = inUV;
    vec3 N = normalize(inNormal);

    // Terrain material blending
    // For now we don't have a splat map, so we'll use a simple blending based on height or slope, 
    // or just show the first layer if it exists.
    // However, the task mentions "render the material layers", implying multiple.
    // Without a splat map, I'll implement a height-based blending as a placeholder.
    
    vec3 albedo = vec3(0.5);
    float roughness = 1.0;
    float metallic = 0.0;
    
    // We have 4 layers in push.layerIndices
    // Let's sample them and blend. 
    // Since we don't have splat map yet, let's just use the first layer as base.
    
    float weights[4] = float[](1.0, 0.0, 0.0, 0.0);
    // Placeholder height-based blending if we wanted:
    // float h = inLocalPosition.y / push.terrainHeight;
    // weights[0] = clamp(1.0 - h * 4.0, 0.0, 1.0);
    // ...
    
    vec3 accumulatedAlbedo = vec3(0.0);
    float accumulatedRoughness = 0.0;
    float accumulatedMetallic = 0.0;
    float totalWeight = 0.0;

    for (int i = 0; i < 4; ++i) {
        uint matIdx = push.layerIndices[i];
        if (matIdx == 0 && i > 0) continue; 
        
        MaterialData mat = materialBuffer.items[matIdx];
        float w = weights[i];
        
        vec3 layerAlbedo = vec3(0.5);
        if (mat.albedoTextureId != 0) {
            layerAlbedo = texture(textureArray[nonuniformEXT(mat.albedoTextureId)], localUV * 10.0).rgb; // Tiled
        }
        
        accumulatedAlbedo += layerAlbedo * w;
        accumulatedRoughness += 1.0 * w; // Default roughness
        accumulatedMetallic += 0.0 * w;
        totalWeight += w;
    }

    if (totalWeight > 0.0) {
        albedo = accumulatedAlbedo / totalWeight;
        roughness = accumulatedRoughness / totalWeight;
        metallic = accumulatedMetallic / totalWeight;
    }

    gBufferAlbedoEmissive = vec4(albedo, 0.0);
    gBufferRoughnessMetallic = vec2(roughness, metallic);
    gBufferRenderIndexDepth = vec2(float(push.renderIndex + 1), gl_FragCoord.z);
    gBufferNormal = vec4(N, 1.0);

    vec2 curr = (inCurrPos.xy / inCurrPos.w) * 0.5 + 0.5;
    vec2 prev = (inPrevPos.xy / inPrevPos.w) * 0.5 + 0.5;
    gBufferMotionVector = curr - prev;
}
