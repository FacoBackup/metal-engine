#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../rt/DisneyBSDF.glsl"
#include "../Atmosphere.glsl"
#include "../rt/SimplifiedBRDF.glsl"

vec3 calculatePixelColor(in vec2 texCoords, MaterialInfo material, HitData interaction) {
    vec3 L = vec3(0.);
    vec3 beta = vec3(1.);

    vec3 wi;

    // Direct lighting - Disney BSDF
    vec3 X = vec3(0.), Y = vec3(0.);
    directionOfAnisotropicity(interaction.hitNormal, X, Y);
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
            float bias = max(.05, 1e-4 * length(interaction.hitPosition));
            vec3 point =  interaction.hitPosition + bias * interaction.hitNormal;
            L += (material.baseColor / PI) * calculateIndirectLighting(material.roughness, interaction.hitNormal, point, rayDirection) * globalData.giStrength;
        }
    }

    return L / globalData.giSamples;
}