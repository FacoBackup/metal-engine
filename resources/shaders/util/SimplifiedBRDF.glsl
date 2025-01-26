struct BounceInfo {
    vec3 albedo;
    bool isEmissive;
    vec3 hitNormal;
    vec3 currentPosition;
    vec3 throughput;
    vec3 indirectLight;
};

void evaluateLightSimplified(in Light l, in BounceInfo bounceInfo, inout vec3 throughput, inout vec3 indirectLight){
    float bias = max(.05, 1e-4 * length(bounceInfo.currentPosition.xyz));
    vec3 localHitPosition = bounceInfo.currentPosition.xyz + bounceInfo.hitNormal * bias;
    vec3 lightDir = normalize(l.position - localHitPosition);
    Ray ray = Ray(localHitPosition, lightDir, 1./lightDir);
    SurfaceInteraction hitData = traceAllTiles(ray);
    float lightDistance = length(l.position - localHitPosition);
    float hitDistance = length(hitData.point - localHitPosition);
    float shadows = 1;
    if (hitData.anyHit && hitDistance < lightDistance) {
        shadows = 0;// clamp(1 / lightDistance, 0, 1)
    }
    if (shadows != 0){
        float NdotL = max(dot(bounceInfo.hitNormal, lightDir), 0.0);
        vec3 lightColorContribution = l.color * bounceInfo.albedo / PI;
        vec3 lightContribution = lightColorContribution * NdotL * shadows / length(l.position - localHitPosition);
        indirectLight += throughput * lightContribution;
        throughput *= lightColorContribution;
    }
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

        if (globalData.lightCount > 0){
            for (int i = 0; i < globalData.lightCount; ++i) {
                Light l = lightsBuffer.lights[i];
                evaluateLightSimplified(l, bounceInfo, lT, lIndirect);
            }
        }

//        vec3 accumulatedResult = previousCacheData.rgb * (1. - 1./accumulationCount) + lIndirect * 1./accumulationCount;
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
    MaterialInfo material;

    float localRoughness = roughness;
    vec3 localRayDir = rayDir;

    for (uint j = 0; j < globalData.giBounces; j++){
        vec3 diffuseRayDir = normalize(bounceInfo.hitNormal + RandomUnitVector());
        vec3 specularRayDir = reflect(localRayDir, bounceInfo.hitNormal);

        specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, localRoughness * localRoughness));
        localRayDir = mix(diffuseRayDir, specularRayDir, 1. - localRoughness);
        Ray ray = Ray(bounceInfo.currentPosition, localRayDir, 1.0 / localRayDir);
        SurfaceInteraction interaction = traceAllTiles(ray);
        if (!interaction.anyHit) {
            if (globalData.isAtmosphereEnabled){
                bounceInfo.albedo = calculate_sky_luminance_rgb(normalize(globalData.sunPosition), ray.d, 2.0f) * 0.05f;
                bounceInfo.isEmissive = true;
                fetchSurfaceCacheRadiance(bounceInfo);
            }
            break;
        }

        localRoughness = 1;

        unpackVoxel(interaction, material);

        bounceInfo.albedo = material.baseColor;
        bounceInfo.isEmissive = material.isEmissive;
        bounceInfo.hitNormal = interaction.normal;
        bounceInfo.currentPosition = interaction.point;

        fetchSurfaceCacheRadiance(bounceInfo);
    }
    return bounceInfo.indirectLight / globalData.giBounces;
}