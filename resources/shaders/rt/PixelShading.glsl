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

            if (bounce > 0) {
                ivec2 coord = ivec2(worldToSurfaceCacheHash(currentHit.hitPosition) *
                vec2(globalData.surfaceCacheWidth, globalData.surfaceCacheHeight));
                vec4 cacheData = imageLoad(giSurfaceCacheCompute, coord);

                if (cacheData.a >= globalData.surfaceCacheMinSamples) {
                    sampleRadiance += throughput * cacheData.rgb;
                    break;
                } else {
// TODO
                }
            }

            vec3 f = vec3(0.0);
            float scatteringPdf = 0.0;
            vec3 directLighting = vec3(0.0);
            for (uint i = 0u; i < globalData.volumesOffset; i++) {
                LightVolume l = lightVolumeBuffer.items[i];
                l.color.rgb *= 20.0;
                directLighting += throughput * calculateDirectLight(l, currentHit, currentMaterial, currentRayDir, f, scatteringPdf) * l.color.a;
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
