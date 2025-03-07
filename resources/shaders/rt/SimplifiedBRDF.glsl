#include "../rt/LightVisibility.glsl"
#include "../rt/RayTracer.glsl"
#include "../rt/RTStructures.glsl"

void evaluateLightSimplified(in LightVolume l, in BounceInfo bounceInfo, inout vec3 throughput, inout vec3 indirectLight){
    float bias = max(.05, 1e-4 * length(bounceInfo.currentPosition.xyz));
    vec3 localHitPosition = bounceInfo.currentPosition.xyz + bounceInfo.hitNormal * bias;
    vec3 lightDir = normalize(l.position - localHitPosition);

    vec3 visibility = visibilityTest(l, bounceInfo.currentPosition.xyz, lightDir);
    float NdotL = max(dot(bounceInfo.hitNormal, lightDir), 0.0);
    vec3 lightColorContribution = l.color.rgb * l.color.a * bounceInfo.albedo * visibility / PI;
    vec3 lightContribution = lightColorContribution * NdotL / length(l.position - localHitPosition);
    indirectLight += throughput * lightContribution;
    throughput *= lightColorContribution;
}

void fetchSurfaceCacheRadiance(inout BounceInfo bounceInfo){
    ivec2 coord = ivec2(worldToSurfaceCacheHash(bounceInfo.currentPosition) * vec2(globalData.surfaceCacheWidth, globalData.surfaceCacheHeight));

    vec4 previousCacheData;

    if (!bounceInfo.isEmissive){
        previousCacheData = imageLoad(giSurfaceCacheCompute, coord);
    }

    if (previousCacheData.a == 1){
        vec3 lIndirect = vec3(0);
        vec3 lT = vec3(1);

        if (globalData.lightVolumeCount > 0){
            for (int i = 0; i < globalData.volumesOffset; ++i) {
                LightVolume l = lightVolumeBuffer.items[i];
                evaluateLightSimplified(l, bounceInfo, lT, lIndirect);
            }
        }

        imageStore(giSurfaceCacheCompute, coord, vec4(lIndirect, 0));
        bounceInfo.indirectLight += lIndirect;
        bounceInfo.throughput *= lT;
    } else {
        if (bounceInfo.isEmissive){
            vec3 scaledAlbedo = bounceInfo.albedo * globalData.giEmissiveFactor;
            bounceInfo.indirectLight += bounceInfo.throughput * scaledAlbedo;
            bounceInfo.throughput *= scaledAlbedo;
        } else {
            bounceInfo.indirectLight += previousCacheData.rgb;
            bounceInfo.throughput *= previousCacheData.rgb;
        }
    }
}

vec3 calculateIndirectLighting(float roughness, vec3 normal, vec3 currentPosition, vec3 rayDir) {
    if (globalData.giBounces == 0) return vec3(0);

    BounceInfo bounceInfo;
    bounceInfo.indirectLight = vec3(0);
    bounceInfo.throughput = vec3(1);
    bounceInfo.currentPosition = currentPosition;
    bounceInfo.hitNormal = normal;

    float localRoughness = roughness;
    vec3 localRayDir = rayDir;

    for (uint j = 0; j < globalData.giBounces; j++){
        vec3 diffuseRayDir = normalize(bounceInfo.hitNormal + RandomUnitVector());
        vec3 specularRayDir = reflect(localRayDir, bounceInfo.hitNormal);

        specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, localRoughness * localRoughness));
        localRayDir = mix(diffuseRayDir, specularRayDir, 1. - localRoughness);
//        float bias = max(.05, 1e-4 * length(bounceInfo.currentPosition));
//        vec3 point =  bounceInfo.currentPosition + bias * bounceInfo.hitNormal;
        HitData interaction = trace(bounceInfo.currentPosition, localRayDir);
        if (!interaction.didHit) {
            if (globalData.isAtmosphereEnabled){
                bounceInfo.albedo = calculate_sky_luminance_rgb(normalize(globalData.sunPosition), bounceInfo.currentPosition, 2.0f) * 0.05f;
                bounceInfo.isEmissive = true;
                fetchSurfaceCacheRadiance(bounceInfo);
            }
            break;
        }

        MaterialInfo material = getMaterialInfo(interaction);
        localRoughness = material.roughness;
        bounceInfo.albedo = material.baseColor;
        bounceInfo.isEmissive = material.isEmissive;
        bounceInfo.hitNormal = interaction.hitNormal;
        bounceInfo.currentPosition = interaction.hitPosition;

        fetchSurfaceCacheRadiance(bounceInfo);
    }
    return bounceInfo.indirectLight / globalData.giBounces;
}