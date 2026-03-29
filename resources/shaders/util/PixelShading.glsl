#include "../util/HWRayTracingUtil.glsl"
#include "../Atmosphere.glsl"

#include "../CreateRay.glsl"

struct PathInfo {
    MaterialInfo material;
    SurfaceInteraction interaction;
    vec3 throughput;
    vec3 radiance;
};

void addDirectLighting(inout PathInfo pathInfo) {
    if (globalData.lightsCount == 0) return;

    int lightIndex = int(random() * globalData.lightsCount);
    Light l = lightBuffer.items[lightIndex];

    vec3 wi, f;
    float scatteringPdf;
    pathInfo.radiance += pathInfo.throughput * calculateDirectLight(l, pathInfo.interaction, pathInfo.material, wi, f, scatteringPdf);
}

vec3 tracePath(vec3 rayDirection, MaterialInfo material, SurfaceInteraction interaction) {
    PathInfo pathInfo;
    pathInfo.radiance = vec3(0);
    pathInfo.throughput = vec3(1);
    pathInfo.material = material;
    pathInfo.interaction = interaction;
    pathInfo.interaction.incomingRayDir = rayDirection;

    // First hit might be emissive
    if (pathInfo.material.isEmissive) {
        return pathInfo.material.baseColor * pushConstants.pathTracingEmissiveFactor;
    }

    for (uint j = 0; j < pushConstants.pathTracerBounces; j++) {
        // Direct Lighting
        addDirectLighting(pathInfo);

        // Sample next direction
        vec3 wi;
        float pdf;
        vec3 X = vec3(0.), Y = vec3(0.);
        directionOfAnisotropicity(pathInfo.interaction.normal, X, Y);
        pathInfo.interaction.tangent = X;
        pathInfo.interaction.binormal = Y;

        vec3 f = bsdfSample(wi, -pathInfo.interaction.incomingRayDir, X, Y, pdf, pathInfo.interaction, pathInfo.material);
        f *= abs(dot(wi, pathInfo.interaction.normal));

        if (pdf < EPSILON || dot(f, f) < EPSILON) break;

        pathInfo.throughput *= f / pdf;

        // Russian Roulette
        if (j >= 3) {
            float q = max(0.05, 1.0 - max(pathInfo.throughput.r, max(pathInfo.throughput.g, pathInfo.throughput.b)));
            if (random() < q) break;
            pathInfo.throughput /= (1.0 - q);
        }

        float bias = 0.001;
        vec3 rayOrigin = pathInfo.interaction.point + (dot(wi, pathInfo.interaction.normal) > 0.0 ? 1.0 : -1.0) * pathInfo.interaction.normal * bias;

        payload.hit = false;
        traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT, 0xFF, 0, 0, 0, rayOrigin, 0.001, wi, 10000.0, 0);

        if (!payload.hit) {
            if (pushConstants.isAtmosphereEnabled != 0) {
                vec3 skyLuminance = calculate_sky_luminance_rgb(normalize(globalData.sunPosition), wi, 2.0f) * 0.05f * pushConstants.pathTracingEmissiveFactor;
                pathInfo.radiance += pathInfo.throughput * skyLuminance;
            }
            break;
        }

        pathInfo.material = payload.material;
        pathInfo.interaction.normal = payload.hitNormal;
        pathInfo.interaction.point = payload.hitPosition;
        pathInfo.interaction.incomingRayDir = wi;

        if (pathInfo.material.isEmissive) {
//            pathInfo.radiance += pathInfo.throughput * pathInfo.material.baseColor * pushConstants.pathTracingEmissiveFactor;
            break; // Stop at emissive for now, or continue if you want multiple emissive hits
        }
    }
    return pathInfo.radiance;
}

vec3 calculatePixelColor(vec3 rayDirection, in vec2 texCoords, MaterialInfo material, SurfaceInteraction interaction) {
    vec3 L = vec3(0.);

    material.subsurface = 0.;
    material.specular = 0.;
    material.specularTint = 0.;
    material.clearcoat = 0.;
    material.clearcoatGloss = 1.;
    material.anisotropic = 0.;
    material.sheen = 0.;
    material.sheenTint = 0.;

    for (uint i = 0; i < pushConstants.pathTracerSamples; i++) {
        L += tracePath(rayDirection, material, interaction);
    }

    return L / pushConstants.pathTracerSamples;
}