#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../rt/DisneyBSDF.glsl"
#include "../Atmosphere.glsl"

vec3 calculatePixelColor(in vec2 texCoords, MaterialInfo material, HitData interaction) {
    vec3 L = vec3(0.0);
    const int maxBounces = int(globalData.giBounces);

    for (uint s = 0u; s < globalData.giSamples; s++) {
        vec3 throughput = vec3(1.0);
        vec3 sampleRadiance = vec3(0.0);

        HitData currentHit = interaction;
        MaterialInfo currentMaterial = material;
        vec3 currentRayDir = currentHit.incomingRayDir;

        for (int bounce = 0; bounce <= maxBounces; bounce++) {
            vec3 tangent, binormal;
            directionOfAnisotropicity(currentHit.hitNormal, tangent, binormal);
            currentHit.tangent  = tangent;
            currentHit.binormal = binormal;

            vec3 f = vec3(0.0);
            float scatteringPdf = 0.0;
            vec3 directLighting = vec3(0.0);

            // --- Surface Cache Integration ---
            if (bounce == 2) { // Only use cache from the 3rd bounce onward
                ivec2 coord = ivec2(worldToSurfaceCacheHash(currentHit.hitPosition) *
                vec2(globalData.surfaceCacheWidth, globalData.surfaceCacheHeight));
                vec4 previousCacheData = imageLoad(giSurfaceCacheCompute, coord);
                float accumulationCount = previousCacheData.a;

                if (accumulationCount >= globalData.surfaceCacheMinSamples) {
                    directLighting = previousCacheData.rgb;// Use cached lighting
                } else {
                    // Compute direct lighting and accumulate it into the cache
                    for (uint i = 0u; i < globalData.volumesOffset; i++) {
                        LightVolume l = lightVolumeBuffer.items[i];
                        l.color.rgb *= 20.0;
                        directLighting += throughput * calculateDirectLight(l, currentHit, currentMaterial, currentRayDir, f, scatteringPdf) * l.color.a;
                    }

                    // Accumulate into the cache
                    vec3 updatedCacheColor = previousCacheData.rgb * (1.0 - 1.0 / accumulationCount) + directLighting * (1.0 / accumulationCount);
                    imageStore(giSurfaceCacheCompute, coord, vec4(updatedCacheColor, accumulationCount + 1.0));
                }
            } else {
                // Compute direct lighting normally for the first and second bounces
                for (uint i = 0u; i < globalData.volumesOffset; i++) {
                    LightVolume l = lightVolumeBuffer.items[i];
                    l.color.rgb *= 20.0;
                    directLighting += throughput * calculateDirectLight(l, currentHit, currentMaterial, currentRayDir, f, scatteringPdf) * l.color.a;
                }
            }

            sampleRadiance += directLighting;

            if (scatteringPdf <= EPSILON || dot(f, f) <= EPSILON) {
                break;
            }
            throughput *= f / scatteringPdf;

            currentHit = trace(currentHit.hitPosition, currentRayDir);
            if (!currentHit.didHit) {
                if (globalData.isAtmosphereEnabled) {
                    sampleRadiance += throughput * calculate_sky_luminance_rgb(normalize(globalData.sunPosition), currentRayDir, 2.0f) * 0.05f;
                }
                break;
            }

            currentMaterial = getMaterialInfo(currentHit);
        }

        L += sampleRadiance;
    }

    return L / float(globalData.giSamples);
}
