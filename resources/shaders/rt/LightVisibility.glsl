#ifndef V_T
#define V_T

#include "../LightVolumeBuffer.glsl"
#include "../rt/RTStructures.glsl"

vec3 visibilityTest(const in LightVolume light,  in vec3 point, vec3 wi) {
    float bias = max(.05, 1e-4 * length(point));
    vec3 shadowsPosition = point + bias * wi;// Offset to avoid self-intersection

    HitData hitData = trace(shadowsPosition, wi);

    float lightDistance = length(light.position - shadowsPosition);
    float hitDistance = length(hitData.hitPosition - shadowsPosition);

    vec3 attenuation = vec3(1);
    // If an opaque surface is blocking the light, return 0 immediately
    if (hitData.didHit && hitDistance < lightDistance) {
        attenuation = vec3(1 / (1 + lightDistance * lightDistance));
    }

    float transmittance = 1.0;
    float distanceInVolume = 0;
    float sigmaT = 0;
    vec3 vColor = vec3(1);
    bool anyHit = false;
    for (uint i = globalData.volumesOffset; i < globalData.lightVolumeCount; i++) {
        LightVolume volume = lightVolumeBuffer.items[i];

        float tEntry, tExit;
        vec3 roLocal = shadowsPosition - volume.position;
        bool intersects = intersectBox(roLocal, wi, volume.dataA, tEntry, tExit);
        if (!intersects) continue;

        if (tEntry > lightDistance){
            continue;
        }

        anyHit = true;
        sigmaT = volume.dataB.x;
        distanceInVolume = tExit - tEntry;
        transmittance *= exp(-sigmaT * distanceInVolume);
        vColor = volume.color.rgb;
        if (transmittance < 1e-3) {
            break;
        }
    }

    return attenuation * vec3(transmittance * exp(-sigmaT * distanceInVolume ) * vColor);
}

#endif