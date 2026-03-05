#ifndef V_T
#define V_T
vec3 visibilityTest(const in Light light, in vec3 point, vec3 wi) {
    float bias = max(.05, 1e-4 * length(point));
    vec3 shadowsPosition = point + bias * wi;// Offset to avoid self-intersection

    float lightDistance = length(light.position - shadowsPosition);

    payload.hit = true; // Default to true, miss shader will set to false
    traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 0, 0, 0, shadowsPosition, 0.001, wi, lightDistance, 0);

    vec3 attenuation = vec3(1);
    // If an opaque surface is blocking the light, fully occlude
    if (payload.hit) {
        attenuation = vec3(0);
    }

    float transmittance = 1.0;
    float distanceInVolume = 0;
    float sigmaT = 0;
    vec3 vColor = vec3(1);
    bool anyHit = false;

    float tEntry, tExit;
    vec3 roLocal = shadowsPosition;
    bool intersects = intersectBox(roLocal, wi, atmosphereData.volumeScale, tEntry, tExit);
    if (intersects && tEntry < lightDistance) {
        anyHit = true;
        sigmaT = atmosphereData.density;
        distanceInVolume = tExit - tEntry;
        transmittance *= exp(-sigmaT * distanceInVolume);
        vColor = atmosphereData.albedo;
    }


    return attenuation * vec3(transmittance * exp(-sigmaT * distanceInVolume) * vColor);
}

#endif