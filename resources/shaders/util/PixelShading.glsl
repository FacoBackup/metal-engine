#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../util/DisneyBSDF.glsl"
#include "../Atmosphere.glsl"
#include "../util/SimplifiedBRDF.glsl"

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

    { // Direct lighting - Disney BSDF
        vec3 X = vec3(0.), Y = vec3(0.);
        directionOfAnisotropicity(interaction.normal, X, Y);
        interaction.tangent = X;
        interaction.binormal = Y;

        vec3 f = vec3(0.);
        float scatteringPdf = 0.;
        vec3 Ld = vec3(0);
        for (uint i = 0; i < globalData.lightCount; i++){
            Light l = lightsBuffer.lights[i];
            l.color *= 50.;
            Ld += beta * calculateDirectLight(l, interaction, material, wi, f, scatteringPdf);
        }

        L += Ld;

        if (scatteringPdf > EPSILON && dot(f, f) > EPSILON)
        beta *=  f / scatteringPdf;

//                wi = normalize(interaction.normal + RandomUnitVector());
        float bias = max(.05, 1e-4 * length(interaction.point));
        interaction.point = interaction.point + bias * interaction.normal;
    }

    return L + (material.baseColor / PI) * calculateIndirectLighting(material, interaction, wi) * globalData.giStrength;
}