#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../util/DisneyBSDF.glsl"
#include "../Atmosphere.glsl"

vec3 calculatePixelColor(in vec2 texCoords, MaterialInfo material, SurfaceInteraction interaction) {
    vec3 L = vec3(0.);
    vec3 beta = vec3(1.);

    vec3 wi;
    vec3 rayDir = createRay(texCoords, globalData.invProj, globalData.invView);
    interaction.incomingRayDir = rayDir;

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

    Ray ray;
    for (float depth = 0.; depth < globalData.giBounces; ++depth) {
        if (depth > 0) {
            interaction = traceAllTiles(ray);
            if (!interaction.anyHit) {
                if (globalData.isAtmosphereEnabled){
                    L += beta * calculate_sky_luminance_rgb(normalize(globalData.sunPosition), ray.d, 2.0f) * 0.05f;
                }
                break;
            } else {
                unpackVoxel(interaction, material);
            }
        }

        vec3 X = vec3(0.), Y = vec3(0.);
        directionOfAnisotropicity(interaction.normal, X, Y);
        interaction.tangent = X;
        interaction.binormal = Y;

        vec3 wi;
        vec3 f = vec3(0.);
        float scatteringPdf = 0.;
        vec3 Ld = vec3(0);
        for (uint i = 0; i < globalData.lightCount; i++){
            Ld += beta * calculateDirectLight(lightsBuffer.lights[i], interaction, material, wi, f, scatteringPdf);
        }

        L += Ld;

        if (scatteringPdf > EPSILON && dot(f, f) > EPSILON)
        beta *=  f / scatteringPdf;

//        ray.d = normalize(interaction.normal + RandomUnitVector());
        ray.d = wi;
        ray.invDir = 1./ray.d;
        float bias = max(.05, 1e-4 * length(interaction.point));
        ray.o = interaction.point + bias * ray.d;
    }

    return L;
}