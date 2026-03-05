#include "../util/HWRayTracingUtil.glsl"
#include "../Atmosphere.glsl"

#include "../CreateRay.glsl"

struct BounceInfo {
    MaterialInfo material;
    SurfaceInteraction interaction;
    vec3 throughput;
    vec3 indirectLight;
};

void computeRadiance(inout BounceInfo bounceInfo)
{
    if (bounceInfo.material.isEmissive)
    {
        vec3 emission = bounceInfo.material.baseColor * pushConstants.pathTracingEmissiveFactor;
        bounceInfo.indirectLight += bounceInfo.throughput * emission;
        return;
    }

    vec3 directRadiance = vec3(0);

    for (int i = 0; i < int(globalData.lightsCount); ++i)
    {
        Light l = lightBuffer.items[i];
        l.color.rgb *= l.color.a;

        vec3 wi;
        vec3 f;
        float scatteringPdf;

        directRadiance += calculateDirectLight(l, bounceInfo.interaction, bounceInfo.material, wi, f, scatteringPdf);
    }

    bounceInfo.indirectLight += bounceInfo.throughput * directRadiance;
}

vec3 calculateIndirectLighting(MaterialInfo material, SurfaceInteraction interaction) {
    if (pushConstants.pathTracerBounces == 0) return vec3(0);

    BounceInfo bounceInfo;
    bounceInfo.indirectLight = vec3(0);
    bounceInfo.throughput = vec3(1);
    bounceInfo.material = material;
    bounceInfo.interaction = interaction;

    for (uint j = 0; j < pushConstants.pathTracerBounces; j++) {
        vec3 wi;
        float pdf;
        vec3 X = vec3(0.), Y = vec3(0.);
        directionOfAnisotropicity(bounceInfo.interaction.normal, X, Y);
        bounceInfo.interaction.tangent = X;
        bounceInfo.interaction.binormal = Y;

        vec3 f = bsdfSample(wi, -bounceInfo.interaction.incomingRayDir, X, Y, pdf, bounceInfo.interaction, bounceInfo.material);
        f *= abs(dot(wi, bounceInfo.interaction.normal));

        if (pdf < EPSILON || dot(f, f) < EPSILON) break;

        bounceInfo.throughput *= f / pdf;

        float bias = 0.001;
        vec3 rayOrigin = bounceInfo.interaction.point + (dot(wi, bounceInfo.interaction.normal) > 0.0 ? 1.0 : -1.0) * bounceInfo.interaction.normal * bias;

        payload.hit = true;
        traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT, 0xFF, 0, 0, 0, rayOrigin, 0.001, wi, 1000.0, 0);

        if (!payload.hit) {
            if (atmosphereData.isAtmosphereEnabled != 0) {
                bounceInfo.material.baseColor = calculate_sky_luminance_rgb(normalize(atmosphereData.sunPosition), wi, 2.0f) * 0.05f;
                bounceInfo.material.isEmissive = true;
                bounceInfo.interaction.point = rayOrigin + wi * 1000.0; // Placeholder point for atmosphere
                computeRadiance(bounceInfo);
            }
            break;
        }

        bounceInfo.material = payload.material;
        bounceInfo.interaction.normal = payload.hitNormal;
        bounceInfo.interaction.point = payload.hitPosition;
        bounceInfo.interaction.incomingRayDir = wi;

        computeRadiance(bounceInfo);
    }
    return bounceInfo.indirectLight;
}

vec3 calculatePixelColor(vec3 rayDirection, in vec2 texCoords, MaterialInfo material, SurfaceInteraction interaction) {
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

    vec3 X = vec3(0.), Y = vec3(0.);
    directionOfAnisotropicity(interaction.normal, X, Y);
    interaction.tangent = X;
    interaction.binormal = Y;

    for (uint i = 0; i < pushConstants.pathTracerSamples; i++) {
        vec3 f = vec3(0.);
        float scatteringPdf = 0.;
        vec3 Ld = vec3(0);
        for (uint i = 0; i < globalData.lightsCount; i++) {
            Light l = lightBuffer.items[i];
            l.color.rgb *= l.color.a;
            Ld += beta * calculateDirectLight(l, interaction, material, wi, f, scatteringPdf) ;
        }

        L += Ld;

        if (pushConstants.pathTracerBounces > 0 && pushConstants.pathTracerMultiplier > 0) {
            L += beta * calculateIndirectLighting(material, interaction) * pushConstants.pathTracerMultiplier;
        }

        if (scatteringPdf > EPSILON && dot(f, f) > EPSILON) {
            beta *= f / scatteringPdf;

            float bias = 0.001;
            interaction.point += (dot(wi, interaction.normal) > 0.0 ? 1.0 : -1.0) * interaction.normal * bias;
        }
    }

    return L / pushConstants.pathTracerSamples;
}