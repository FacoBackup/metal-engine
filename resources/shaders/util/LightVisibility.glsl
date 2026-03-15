#ifndef V_T
#define V_T
vec3 visibilityTest(float lightDistance, in vec3 point, vec3 wi) {
    float bias = max(.001, 1e-4 * length(point));
    vec3 shadowsPosition = point + bias * wi;// Offset to avoid self-intersection

    payload.hit = true; // Default to true, miss shader will set to false
    traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 0, 0, 0, shadowsPosition, 0.001, wi, lightDistance - 0.002, 0);

    vec3 attenuation = vec3(1);
    // If an opaque surface is blocking the light, fully occlude
    if (payload.hit) {
        attenuation = vec3(0);
    }

    float transmittance = 1.0;
    for (uint i = 0; i < globalData.volumeCount; i++) {
        Volume volume = volumesBuffer.items[i];

        float tEntry, tExit;
        vec3 roLocal = point - volume.position;
        bool intersects = intersectBox(roLocal, wi, volume.dataA, tEntry, tExit);
        if (!intersects) continue;

        tEntry = max(0.0, tEntry);
        tExit = min(lightDistance, tExit);
        
        if (tEntry >= tExit) continue;

        float distanceInVolume = tExit - tEntry;
        float sigmaT = volume.dataB.x;
        transmittance *= exp(-sigmaT * distanceInVolume);
        
        if (transmittance < 1e-3) {
            transmittance = 0.0;
            break;
        }
    }

    return attenuation * transmittance;
}

#endif