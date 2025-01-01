#define PI 3.14159265
#include "./LightsBuffer.glsl"

struct ShaderData {
    vec3 albedo;
    vec3 N;
    vec3 viewSpacePosition;
    vec3 worldSpacePosition;
    vec3 V;
    vec3 VrN;
    vec3 albedoOverPI;
    vec3 F0;
    vec3 sunDirection;
    vec3 sunColor;
    vec2 brdf;
    float NdotV;
    float distanceFromCamera;
    float roughness;
    float metallic;
    float ambientOcclusion;
    uint lightsQuantity;
    bool enabledSun;
};

vec3 getDiffuse(vec3 KS, float metallic) {
    return (1. - KS) * (1. - metallic);
}
vec3 fresnelSchlick(float VdotH, vec3 F0) {
    float f = pow(1.0 - VdotH, 5.0);
    return f + F0 * (1.0 - f);
}

vec3 fresnel(vec3 F0, float F90, float HdotV) {
    return F0 + (F90 - F0) * pow(1.0 - HdotV, 5.0);
}

float distributionGGX(float NdotH, float roughness) {
    float a2 = pow(roughness, 4.);
    float denom = (pow(NdotH, 2.) * (a2 - 1.0) + 1.0);
    return a2 / (PI * pow(denom, 2.));
}

float geometrySmith(in ShaderData shaderData, float NdotL) {
    float roughnessSquared = pow(shaderData.roughness, 2.);
    float V = NdotL * (shaderData.NdotV * (1. - roughnessSquared) + roughnessSquared);
    float L = shaderData.NdotV * (NdotL * (1. - roughnessSquared) + roughnessSquared);
    return clamp(0.5 * 1. / (V + L), 0., 1.);
}

void isotropicCompute(in ShaderData shaderData, inout vec3 dEnergy, inout vec3 specularTotal, inout vec3 L, float NdotL, inout vec3 lightColor, float HdotV, float NdotH) {
    float D = distributionGGX(NdotH, shaderData.roughness);
    float V = geometrySmith(shaderData, NdotL);
    vec3 F = fresnelSchlick(HdotV, shaderData.F0);
    specularTotal += D * V * F;
    dEnergy *= getDiffuse(F, shaderData.metallic);
}

vec3 computeBRDF(in ShaderData shaderData, vec3 L, float NdotL, vec3 lightColor) {
    vec3 H = normalize(shaderData.V + L);
    float HdotV = clamp(dot(H, shaderData.V), 0., 1.);
    float NdotH = clamp(dot(shaderData.N, H), 0., 1.);

    vec3 specularTotal = vec3(0.);
    vec3 diffuseEnergy = vec3(1.);

    isotropicCompute(shaderData, diffuseEnergy, specularTotal, L, NdotL, lightColor, HdotV, NdotH);

    return (shaderData.albedoOverPI * diffuseEnergy + specularTotal) * lightColor * NdotL;
}

vec4 precomputeContribution(in ShaderData shaderData, vec3 lightPosition) {
    vec3 L = normalize(lightPosition - shaderData.worldSpacePosition);
    float NdotL = max(dot(shaderData.N, L), 0.0);
    if (NdotL <= 0.) return vec4(0.);
    return vec4(L, NdotL);
}

vec3 computeDirectionalLight(in ShaderData shaderData){
    vec4 baseContribution = precomputeContribution(shaderData, shaderData.sunDirection.xyz);
    if (baseContribution.a == 0.) return vec3(0.);
    return computeBRDF(shaderData, baseContribution.rgb, baseContribution.a, shaderData.sunColor.rgb);
}


vec3 computeSphereLight(in ShaderData shaderData, in Light info){
    vec3 L = info.position - shaderData.worldSpacePosition;
    vec3 centerToRay        = dot(L, shaderData.VrN) * shaderData.VrN - L;
    vec3 closestPoint        = L + centerToRay * clamp(info.radius / length(centerToRay), 0.0, 1.0);
    vec4 baseContribution = precomputeContribution(shaderData, closestPoint + shaderData.worldSpacePosition);

    if (baseContribution.a == 0.){
        return vec3(0.);
    }

    float distanceFromFrag =  length(L);
    float intensity = 1.;
    float innerRadius = min(info.outerRadius, info.innerRadius);
    if (distanceFromFrag > innerRadius) {
        intensity = clamp(mix(1., 0., (distanceFromFrag - innerRadius) / (info.outerRadius - innerRadius)), 0., 1.);
    }
    if (intensity < .01){
        return vec3(0);
    }
    return computeBRDF(shaderData, baseContribution.rgb, baseContribution.a, info.color) * intensity;
}

vec3 physicallyBasedShadePixel(in ShaderData shaderData) {
    vec3 finalColor = shaderData.enabledSun ? computeDirectionalLight(shaderData) : vec3(0);

    for (uint i = 0; i < shaderData.lightsQuantity; i++){
        finalColor += computeSphereLight(shaderData, lightsBuffer.lights[i]);
    }

    return finalColor * shaderData.ambientOcclusion;
}