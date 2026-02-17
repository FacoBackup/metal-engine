#extension GL_EXT_ray_tracing : require

#include "../util/HWRayTracingUtil.glsl"

layout(location = 0) rayPayloadInEXT RayPayload payload;

void main() {
    payload.hit = true;
    payload.t = gl_HitTEXT;
    payload.hitPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    payload.hitNormal = normalize(-gl_WorldRayDirectionEXT);

    payload.material.baseColor = vec3(0.8, 0.2, 0.1);
    payload.material.roughness = 0.5;
    payload.material.metallic = 0.0;
    payload.material.subsurface = 0.0;
    payload.material.specular = 0.0;
    payload.material.specularTint = 0.0;
    payload.material.clearcoat = 0.0;
    payload.material.clearcoatGloss = 1.0;
    payload.material.anisotropic = 0.0;
    payload.material.sheen = 0.0;
    payload.material.sheenTint = 0.0;
    payload.material.isEmissive = false;
}
