#ifndef V_T
#define V_T

#include "../LightsBuffer.glsl"
#include "../VolumesBuffer.glsl"
#include "../rt/RTStructures.glsl"

vec3 visibilityTest(const in LightInstance light, in vec3 point, vec3 wi) {
    HitData hitData = trace(point, wi);
    float lightDistance = length(light.position - point);
    float hitDistance = length(hitData.hitPosition - point);

    vec3 attenuation = vec3(1.0);

    if (hitData.didHit && hitDistance < lightDistance) {
        MaterialInfo hitMaterial = getMaterialInfo(hitData);
        if (hitMaterial.transmission > 0.0) {
            // Compute transmittance for transparent surfaces
            float fresnel = fresnelSchlick(abs(dot(hitData.hitNormal, wi)), hitMaterial.ior);
            vec3 absorptionFactor = exp(-hitMaterial.absorption * hitDistance) * hitMaterial.baseColor;
            attenuation *= mix(absorptionFactor, vec3(0.0), fresnel * (1.0 - hitMaterial.transmission)) ;
        } else {
            // Fully opaque surface blocks light
            attenuation = vec3(0.0);
        }
    }

    float transmittance = 1.0;
    float distanceInVolume = 0.0;
    float sigmaT = 0.0;
    vec3 vColor = vec3(1.0);
    bool anyHit = false;

    for (uint i = 0; i < globalData.volumeCount; i++) {
        VolumeInstance volume = volumesBuffer.items[i];
        float tEntry, tExit;
        vec3 roLocal = point - volume.position;
        bool intersects = intersectBox(roLocal, wi, volume.size, tEntry, tExit);
        if (!intersects) continue;

        if (tEntry > lightDistance) continue;

        anyHit = true;
        sigmaT = volume.density;
        distanceInVolume = tExit - tEntry;
        transmittance *= exp(-sigmaT * distanceInVolume);
        vColor = volume.color.rgb;

        if (transmittance < 1e-3) break;
    }

    return attenuation * vec3(transmittance * exp(-sigmaT * distanceInVolume) * vColor);
}

#endif