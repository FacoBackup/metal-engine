#include "./GlobalDataBuffer.glsl"
#include "./GBufferGenPushConstant.glsl"
#include "./GBufferUtil.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 1, binding = 0) uniform sampler2D albedoEmissiveSampler;
layout(set = 2, binding = 0) uniform sampler2D normalSampler;
layout(set = 3, binding = 0) uniform sampler2D roughnessSampler;
layout(set = 4, binding = 0) uniform sampler2D metallicSampler;
layout(set = 5, binding = 0) uniform sampler2D aoSampler;
layout(set = 6, binding = 0) uniform sampler2D heightMapSampler;

layout (location = 0) out vec4 outMaterialA;
layout (location = 1) out vec4 outMaterialB;
layout (location = 2) out vec4 outMaterialC;

#ifdef DEBUG
#include "./DebugFlags.glsl"
#endif

float encode(float depthFunc, float val) {
    return log2(max(0.000001, val)) * depthFunc * 0.5;
}

mat3 computeTBN(vec3 worldPosition, vec2 initialUV, vec3 normalVec, bool isDecalPass) {
    if (isDecalPass) {
        vec3 N = abs(normalVec);
        vec3 T = vec3(0., 0., 1.);
        if (N.z > N.x && N.z > N.y){
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
vec2 parallaxOcclusionMapping(vec2 initialUV, vec3 worldSpacePosition, float heightScale, int layers, float distanceFromCamera, mat3 TBN) {
    if (distanceFromCamera > PARALLAX_THRESHOLD) return initialUV;
    mat3 transposed = transpose(TBN);
    vec3 viewDirection = normalize(transposed * (globalData.cameraWorldPosition.xyz - worldSpacePosition.xyz));
    float fLayers = float(max(layers, 1));
    float layerDepth = 1.0 / fLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDirection.xy / viewDirection.z * max(heightScale, .00000001);
    vec2 deltaTexCoords = P / fLayers;

    vec2 currentUVs = initialUV;
    float currentDepthMapValue = texture(heightMapSampler, currentUVs).r;
    while (currentLayerDepth < currentDepthMapValue) {
        currentUVs -= deltaTexCoords;
        currentDepthMapValue = texture(heightMapSampler, currentUVs).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentUVs + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(heightMapSampler, prevTexCoords).r - currentLayerDepth + layerDepth;

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

void main () {
    bool isDecalPass = false;// TODO - MOVE TO PUSH CONSTANT
    vec2 localUV = inUV;
    vec3 N = normalize(inNormal);
    mat3 TBN = computeTBN(inPosition, localUV, N, isDecalPass);
    vec3 W = inPosition;
    if (push.useHeightTexture){
        vec3 V = globalData.cameraWorldPosition.xyz - inPosition;
        float distanceFromCamera = length(V);
        localUV = parallaxOcclusionMapping(localUV, W, push.parallaxHeightScale, push.parallaxLayers, distanceFromCamera, TBN);

    }
    float metallic = push.metallicFactor;
    float roughness = push.roughnessFactor;
    outMaterialA = vec4(push.albedoEmissive.rgb, gl_FragCoord.z);
    outMaterialB = vec4(N, 0);
    outMaterialC = vec4(inPosition, push.renderIndex + 1);
    if (push.useAlbedoTexture){
        outMaterialA.rgb = texture(albedoEmissiveSampler, localUV).rgb;
    }

    if (push.useNormalTexture){
        outMaterialB.rgb = vec3(normalize(TBN * (texture(normalSampler, localUV).rgb * 2 - 1)));
    }

    if (push.useRoughnessTexture){
        roughness = texture(roughnessSampler, localUV).r;
    }

    if (push.useMetallicTexture){
        metallic = texture(metallicSampler, localUV).r;
    }

    if (push.useAOTexture){
        outMaterialA.a *= texture(aoSampler, localUV).r;
    }
    outMaterialA.a *= push.albedoEmissive.a > 0 ? -1 : 1;
    outMaterialB.a = compressRoughnessMetallic(metallic, roughness);

    #ifdef DEBUG
    if (globalData.debugFlag == UV) {
        outMaterialA = vec4(normalize(localUV), 0, 1);
    } else if (globalData.debugFlag == RANDOM) {
        outMaterialA = vec4(randomColor(int(push.renderIndex + 1)), -1);
    }
    #endif
}