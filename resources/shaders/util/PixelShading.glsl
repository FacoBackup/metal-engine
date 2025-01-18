#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"
#include "../util/DisneyBSDF.glsl"

vec3 calculatePixelColor(in vec2 texCoords, MaterialInfo material, SurfaceInteraction interaction) {
    vec3 L = vec3(0.);
    vec3 beta = vec3(1.);

    state = uint(uint(gl_GlobalInvocationID.x) * uint(1973) + uint(gl_GlobalInvocationID.y) * uint(9277) + uint(globalData.globalFrameCount) * uint(26699)) | uint(1);

    vec3 wi;
    vec3 rayDir = createRay(texCoords, globalData.invProj, globalData.invView);
    Ray ray = Ray(globalData.cameraWorldPosition, rayDir, 1/rayDir);
    interaction.incomingRayDir = rayDir;
    for (float depth = 0.; depth < MAXDEPTH; ++depth) {
        if(depth == 0){
            // Sample GBuffer
        } else {
            interaction = traceAllTiles(ray);
            if (!interaction.anyHit) {
                //            L += beta * gammaToLinear(texture(iChannel0, ray.direction).rgb);
                // Atmosphere
                break;
            }else{
                material = unpackVoxel(interaction);
            }
        }

        vec3 X = vec3(0.), Y = vec3(0.);
        directionOfAnisotropicity(interaction.normal, X, Y);
        interaction.tangent = X;
        interaction.binormal = Y;

        vec3 wi;
        vec3 f = vec3(0.);
        float scatteringPdf = 0.;
        vec3 Ld = beta * calculateDirectLight(lightsBuffer.lights[0], interaction, material, wi, f, scatteringPdf);
//        Ld += beta * calculateDirectLight(lightsBuffer.lights[1], interaction, material, wi, f, scatteringPdf);

        L += Ld;

        if( scatteringPdf > EPSILON && dot(f, f) > EPSILON)
        beta *=  f / scatteringPdf;

        ray.d = wi;
        ray.o = interaction.point + .02 * ray.d;
    }

    return L;
}