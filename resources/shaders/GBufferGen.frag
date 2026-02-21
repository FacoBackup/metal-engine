#extension GL_EXT_nonuniform_qualifier: enable
#include "./GlobalDataBuffer.glsl"
#include "./GBufferGenPushConstant.glsl"
#include "./GBufferUtil.glsl"
#define MATERIAL_SET 1
#include "./MaterialBuffer.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (set = 2, binding = 0) uniform sampler2D textureArray[];

layout (location = 0) out vec4 outMaterialA;
layout (location = 1) out vec4 outMaterialB;
layout (location = 2) out vec4 outMaterialC;

#ifdef DEBUG
#include "./DebugFlags.glsl"
#endif

bool hasMaterial() { return push.materialIndex != 0u; }

float encode(float depthFunc, float val) {
    return log2(max(0.000001, val)) * depthFunc * 0.5;
}

mat3 computeTBN(vec3 worldPosition, vec2 initialUV, vec3 normalVec, bool isDecalPass) {
    if (isDecalPass) {
        vec3 N = abs(normalVec);
        vec3 T = vec3(0., 0., 1.);
        if (N.z > N.x && N.z > N.y) {
            T = vec3(1., 0., 0.);
        }

        T = normalize(T - N * dot(T, N));
        vec3 B = cross(T, N);
        return mat3(T, B, N);
    }
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
vec2 parallaxOcclusionMapping(vec2 initialUV, vec3 worldSpacePosition, float heightScale, int layers, float distanceFromCamera, mat3 TBN, uint heightTextureIndex) {
    if (distanceFromCamera > PARALLAX_THRESHOLD) return initialUV;
    mat3 transposed = transpose(TBN);
    vec3 viewDirection = normalize(transposed * (globalData.cameraWorldPosition.xyz - worldSpacePosition.xyz));
    float fLayers = float(max(layers, 1));
    float layerDepth = 1.0 / fLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDirection.xy / viewDirection.z * max(heightScale, .00000001);
    vec2 deltaTexCoords = P / fLayers;

    vec2 currentUVs = initialUV;
    float currentDepthMapValue = texture(textureArray[nonuniformEXT(heightTextureIndex)], currentUVs).r;
    while (currentLayerDepth < currentDepthMapValue) {
        currentUVs -= deltaTexCoords;
        currentDepthMapValue = texture(textureArray[nonuniformEXT(heightTextureIndex)], currentUVs).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentUVs + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(textureArray[nonuniformEXT(heightTextureIndex)], prevTexCoords).r - currentLayerDepth + layerDepth;

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

void main() {
    bool isDecalPass = false;// TODO - MOVE TO PUSH CONSTANT
    vec2 localUV = inUV;
    vec3 N = normalize(inNormal);
    mat3 TBN = computeTBN(inPosition, localUV, N, isDecalPass);
    vec3 W = inPosition;

    float metallic = push.metallicFactor;
    float roughness = push.roughnessFactor;
    outMaterialA = vec4(push.albedoEmissive.rgb, gl_FragCoord.z);
    outMaterialB = vec4(N, 0);
    outMaterialC = vec4(inPosition, push.renderIndex + 1);
    bool isEmissive = push.albedoEmissive.a > 0;
    if (hasMaterial()) {
        MaterialData mat = materialBuffer.items[push.materialIndex];
        isEmissive = mat.isEmissive == 1u;

        if (mat.useHeightTexture == 1u) {
            vec3 V = globalData.cameraWorldPosition.xyz - inPosition;
            float distanceFromCamera = length(V);
            localUV = parallaxOcclusionMapping(inUV, inPosition, push.parallaxHeightScale, push.parallaxLayers, distanceFromCamera, TBN, mat.heightTexture);
        }

        if (mat.useAlbedoTexture == 1u) {
            outMaterialA.rgb = texture(textureArray[nonuniformEXT(mat.albedoTexture)], localUV).rgb;
        } else {
            outMaterialA.rgb = mat.albedo;
        }

        if (mat.useNormalTexture == 1u) {
            vec3 nrm = texture(textureArray[nonuniformEXT(mat.normalTexture)], localUV).rgb * 2.0 - 1.0;
            outMaterialB.rgb = vec3(normalize(TBN * nrm));
        }

        if (mat.useRoughnessTexture == 1u) {
            roughness = texture(textureArray[nonuniformEXT(mat.roughnessTexture)], localUV).r;
        } else {
            roughness = mat.roughness;
        }

        if (mat.useMetallicTexture == 1u) {
            metallic = texture(textureArray[nonuniformEXT(mat.metallicTexture)], localUV).r;
        } else {
            metallic = mat.metallic;
        }
    }

    outMaterialA.a *= isEmissive ? -1 : 1;
    outMaterialB.a = compressRoughnessMetallic(metallic, roughness);

    #ifdef DEBUG
    if (globalData.debugFlag == UV) {
        outMaterialA = vec4(normalize(localUV), 0, 1);
    } else if (globalData.debugFlag == RANDOM) {
        outMaterialA = vec4(randomColor(int(push.renderIndex + 1)), -1);
    }
    #endif
}