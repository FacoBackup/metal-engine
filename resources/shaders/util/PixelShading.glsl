#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../util/DisneyBSDF.glsl"
#include "../Atmosphere.glsl"
#include "../util/SimplifiedBRDF.glsl"

vec3 calculatePixelColor( vec3 rayDirection, in vec2 texCoords, MaterialInfo material, SurfaceInteraction interaction) {
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
    //    material.clearcoat = 1.;
    //    material.specular = 0.1;
    //    material.subsurface = 1.;
    //    material.clearcoatGloss = 1.;

    // Direct lighting - Disney BSDF
    vec3 X = vec3(0.), Y = vec3(0.);
    directionOfAnisotropicity(interaction.normal, X, Y);
    interaction.tangent = X;
    interaction.binormal = Y;

    for (uint i = 0; i < globalData.giSamples; i++){
        vec3 f = vec3(0.);
        float scatteringPdf = 0.;
        vec3 Ld = vec3(0);
        for (uint i = 0; i < globalData.volumesOffset; i++){
            LightVolume l = lightVolumeBuffer.items[i];
            l.color.rgb *= 20.;
            Ld += beta * calculateDirectLight(l, interaction, material, wi, f, scatteringPdf) * l.color.a;
        }

        L += Ld;

        if (scatteringPdf > EPSILON && dot(f, f) > EPSILON){
            beta *=  f / scatteringPdf;
        }

        if (globalData.giBounces > 0 && globalData.giStrength > 0){
            float bias = max(.05, 1e-4 * length(interaction.point));
            vec3 point =  interaction.point + bias * interaction.normal;
            L += (material.baseColor / PI) * calculateIndirectLighting(material.roughness, interaction.normal, point, rayDirection) * globalData.giStrength;
        }
    }

    return L / globalData.giSamples;
}