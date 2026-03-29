#include "../GlobalDataBuffer.glsl"
#extension GL_EXT_nonuniform_qualifier : enable
#include "../gbuffer/GBufferGenPushConstant.glsl"
#include "../gbuffer/GBufferUtil.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inLocalPosition;

#define MATERIAL_DATA_SET 1
#include "../MaterialData.glsl"
layout(set = 0, binding = 2) uniform sampler2D textureArray[];

layout (location = 0) out vec4 gBufferAlbedoEmissive;
layout (location = 1) out vec2 gBufferRoughnessMetallic;
layout (location = 2) out vec2 gBufferRenderIndexDepth;
layout (location = 3) out vec4 gBufferNormal;



mat3 computeTBN(vec3 worldPosition, vec2 initialUV, vec3 normalVec, bool isDecalPass) {
    vec3 dp1 = dFdx(worldPosition);
    vec3 dp2 = dFdy(worldPosition);
    vec2 duv1 = dFdx(initialUV);
    vec2 duv2 = dFdy(initialUV);

    vec3 dp2perp = cross(dp2, normalVec);
    vec3 dp1perp = cross(normalVec, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, normalVec);
}

#define PARALLAX_THRESHOLD 200.
vec2 parallaxOcclusionMapping(vec2 initialUV, vec3 worldSpacePosition, float heightScale, int layers, float distanceFromCamera, mat3 TBN, uint heightTextureId) {
    if (distanceFromCamera > PARALLAX_THRESHOLD || heightTextureId == 0) return initialUV;
    mat3 transposed = transpose(TBN);
    vec3 viewDirection = normalize(transposed * (globalData.cameraWorldPosition.xyz - worldSpacePosition.xyz));
    
    float fLayers = float(max(layers, 1));
    float layerDepth = 1.0 / fLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDirection.xy / viewDirection.z * max(heightScale, .00000001);
    vec2 deltaTexCoords = P / fLayers;

    vec2 currentUVs = initialUV;
    float currentDepthMapValue = texture(textureArray[nonuniformEXT(heightTextureId)], currentUVs).r;
    while (currentLayerDepth < currentDepthMapValue) {
        currentUVs -= deltaTexCoords;
        currentDepthMapValue = texture(textureArray[nonuniformEXT(heightTextureId)], currentUVs).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentUVs + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(textureArray[nonuniformEXT(heightTextureId)], prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    return prevTexCoords * weight + currentUVs * (1.0 - weight);
}


#ifdef DEBUG
vec3 randomColor(int seed) {
    float hash = fract(sin(float(seed)) * 43758.5453);

    float r = fract(hash * 13.756);
    float g = fract(hash * 15.734);
    float b = fract(hash * 17.652);

    return vec3(r, g, b);
}
#endif

struct GridData {
    vec3 baseColor;
    float roughness;
    float metallic;
};

float sharpGrid(vec3 pos, float lineWeight) {
    vec3 aaf = fwidth(pos);
    vec3 grid3 = smoothstep(0.5 - lineWeight - aaf, 0.5 - lineWeight + aaf, abs(fract(pos) - 0.5));
    return max(grid3.x, max(grid3.y, grid3.z));
}

GridData getDebugGrid(vec3 localPos) {
    vec3 checkerPos = floor(localPos * 0.5);
    float checker = mod(checkerPos.x + checkerPos.y + checkerPos.z, 2.0);

    vec3 colorA = vec3(0.5);
    vec3 colorB = vec3(0.6);
    vec3 base = mix(colorA, colorB, checker);

    float mainGrid = sharpGrid(localPos, 0.01);
    float subGrid = sharpGrid(localPos * 10.0, 0.005);

    vec3 finalColor = mix(base, vec3(0.4), subGrid * 0.5);
    finalColor = mix(finalColor, vec3(0.25), mainGrid);

    float lineStrength = max(mainGrid, subGrid * 0.5);

    GridData data;
    data.baseColor = finalColor;
    data.roughness = mix(1.0, 0.1, lineStrength);
    data.metallic = mix(0.0, 1.0, lineStrength);
    return data;
}

void main () {
    vec2 localUV = inUV;
    vec3 N = normalize(inNormal);
    mat3 TBN = computeTBN(inPosition, localUV, N, false);

    MaterialData material = materialBuffer.items[push.renderIndex];

    if (material.heightTextureId != 0){
        vec3 V = globalData.cameraWorldPosition.xyz - inPosition;
        float distanceFromCamera = length(V);
        localUV = parallaxOcclusionMapping(localUV, inPosition, push.parallaxHeightScale, push.parallaxLayers, distanceFromCamera, TBN, material.heightTextureId);
    }

    GridData grid = getDebugGrid(inLocalPosition);
    vec3 albedo = grid.baseColor;
    if (material.albedoTextureId != 0){
        albedo = texture(textureArray[nonuniformEXT(material.albedoTextureId)], localUV).rgb;
    }

    if (material.normalTextureId != 0){
        N = normalize(TBN * (texture(textureArray[nonuniformEXT(material.normalTextureId)], localUV).rgb * 2 - 1));
    }

    float roughness = grid.roughness;
    if (material.roughnessTextureId != 0){
        roughness = texture(textureArray[nonuniformEXT(material.roughnessTextureId)], localUV).r;
    }

    float metallic = grid.metallic;
    if (material.metallicTextureId != 0){
        metallic = texture(textureArray[nonuniformEXT(material.metallicTextureId)], localUV).r;
    }

    #ifdef DEBUG
    if (globalData.debugFlag != LIT) {
        if (globalData.debugFlag == NORMAL) {
            albedo = normalize(N);
        } else if (globalData.debugFlag == DEPTH) {
            albedo = vec3(gl_FragCoord.z);
        } else if (globalData.debugFlag == UV) {
            albedo = vec3(localUV, 0);
        } else if (globalData.debugFlag == RANDOM) {
            albedo = randomColor(int(push.renderIndex + 1));
        } else if (globalData.debugFlag == POSITION) {
            albedo = normalize(inPosition);
        } else if (globalData.debugFlag == ALBEDO) {
            albedo = albedo;
        } else if (globalData.debugFlag == ROUGHNESS) {
            albedo = vec3(roughness);
        } else if (globalData.debugFlag == METALLIC) {
            albedo = vec3(metallic);
        } else if (globalData.debugFlag == EMISSIVE) {
            albedo = vec3(material.isEmissive == 1u ? 1.0 : 0.0);
        }
    } else if (globalData.debugFlag == GRID) {
        albedo = grid.baseColor;
        roughness = grid.roughness;
        metallic = grid.metallic;
    }
    #endif

    gBufferAlbedoEmissive = vec4(albedo, material.isEmissive == 1u ? 1.0 : 0.0);

    #ifdef DEBUG
    if (globalData.debugFlag != LIT && globalData.debugFlag != GRID) {
        gBufferAlbedoEmissive.a = -1.0;
    }
    #endif

    gBufferRoughnessMetallic = vec2(roughness, metallic);
    gBufferRenderIndexDepth = vec2(float(push.renderIndex + 1), gl_FragCoord.z);
    gBufferNormal = vec4(N, 1.0);
}