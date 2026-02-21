#include "../util/HWRayTracingUtil.glsl"
#include "../Atmosphere.glsl"

vec3 createRay(in vec2 texCoords, in mat4 invProjectionMatrix, in mat4 invViewMatrix) {
    vec2 pxNDS = texCoords * 2. - 1.;
    vec4 pointNDSH = vec4(pxNDS, 0.0, 1.0); // Ensure point is at far plane
    vec4 dirEye = invProjectionMatrix * pointNDSH;
    dirEye.w = 0.0;
    vec3 dirWorld = normalize((invViewMatrix * dirEye).xyz);
    return dirWorld;
}

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
        previousCacheData = imageLoad(surfaceCacheImage, coord);
    }

    if (previousCacheData.a == 1){
        vec3 lIndirect = vec3(0);
        vec3 lT = vec3(1);

        for (int i = 0; i < globalData.lightsCount; ++i) {
            Light l = lightBuffer.items[i];
            evaluateLightSimplified(l, bounceInfo, lT, lIndirect);
        }

        imageStore(surfaceCacheImage, coord, vec4(lIndirect, 0));
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
    if (globalData.pathTracerBounces == 0) return vec3(0);

    BounceInfo bounceInfo;
    bounceInfo.indirectLight = vec3(0);
    bounceInfo.throughput = vec3(1);
    bounceInfo.currentPosition = currentPosition;
    bounceInfo.hitNormal = normal;

    float localRoughness = roughness;
    vec3 localRayDir = rayDir;

    uint samples = 0;
    for (uint j = 0; j < globalData.pathTracerBounces; j++){
        vec3 diffuseRayDir = normalize(bounceInfo.hitNormal + RandomUnitVector());
        vec3 specularRayDir = reflect(localRayDir, bounceInfo.hitNormal);

        specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, localRoughness * localRoughness));
        localRayDir = mix(diffuseRayDir, specularRayDir, 1. - localRoughness);

        payload.hit = true;
        traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT, 0xFF, 0, 0, 0, bounceInfo.currentPosition, 0.001, localRayDir, 1000.0, 0);

        if (!payload.hit) {
            if (globalData.isAtmosphereEnabled){
                bounceInfo.albedo = calculate_sky_luminance_rgb(normalize(globalData.sunPosition), localRayDir, 2.0f) * 0.05f;
                bounceInfo.isEmissive = true;
                fetchSurfaceCacheRadiance(bounceInfo);
                samples++;
            }
            break;
        }

        localRoughness = 1;

        bounceInfo.albedo = payload.material.baseColor;
        bounceInfo.isEmissive = payload.material.isEmissive;
        bounceInfo.hitNormal = payload.hitNormal;
        bounceInfo.currentPosition = payload.hitPosition;

        fetchSurfaceCacheRadiance(bounceInfo);
        samples++;
    }
    return samples > 0 ? bounceInfo.indirectLight / samples : vec3(0);
}

vec3 calculatePixelColor( vec3 rayDirection, in vec2 texCoords, MaterialInfo material, SurfaceInteraction interaction) {
    vec3 L = vec3(0.);
    vec3 beta = vec3(1.);

    vec3 wi;
    interaction.incomingRayDir = rayDirection;

    material.subsurface = 0.;
    material.specular = 0.;
    material.specularTint = 0.;
    material.clearcoat = 0.;
    material.clearcoatGloss = 1.;
    material.anisotropic = 0.;
    material.sheen = 0.;
    material.sheenTint = 0.;
    //    material.clearcoat = 1.;
    //    material.specular = 0.1;
    //    material.subsurface = 1.;
    //    material.clearcoatGloss = 1.;

    // Direct lighting - Disney BSDF
    vec3 X = vec3(0.), Y = vec3(0.);
    directionOfAnisotropicity(interaction.normal, X, Y);
    interaction.tangent = X;
    interaction.binormal = Y;

    for (uint i = 0; i < globalData.pathTracerSamples; i++){
        vec3 f = vec3(0.);
        float scatteringPdf = 0.;
        vec3 Ld = vec3(0);
        for (uint i = 0; i < globalData.lightsCount; i++){
            Light l = lightBuffer.items[i];
            l.color.rgb *= 20.;
            Ld += beta * calculateDirectLight(l, interaction, material, wi, f, scatteringPdf) * l.color.a;
        }

        L += Ld;

        if (scatteringPdf > EPSILON && dot(f, f) > EPSILON){
            beta *=  f / scatteringPdf;
        }

        if (globalData.pathTracerBounces > 0 && globalData.pathTracerMultiplier > 0){
            float bias = max(.05, 1e-4 * length(interaction.point));
            vec3 point =  interaction.point + bias * interaction.normal;
            L += (material.baseColor / PI) * calculateIndirectLighting(material.roughness, interaction.normal, point, rayDirection) * globalData.pathTracerMultiplier;
        }
    }

    return L / globalData.pathTracerSamples;
}