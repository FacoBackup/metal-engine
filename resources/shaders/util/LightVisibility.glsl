#ifndef V_T
#define V_T
vec3 visibilityTest(float lightDistance, in vec3 point, vec3 wi) {
    float bias = max(.001, 1e-4 * length(point));
    vec3 shadowsPosition = point + bias * wi;// Offset to avoid self-intersection

    payload.hit = true; // Default to true, miss shader will set to false
    traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 0, 0, 0, shadowsPosition, 0.001, wi, lightDistance - 0.002, 0);

    // If an opaque surface is blocking the light, fully occlude
    if (payload.hit) {
        return vec3(0);
    }

    return vec3(1);
}

#endif