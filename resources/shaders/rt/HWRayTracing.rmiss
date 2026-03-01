#extension GL_EXT_ray_tracing : require

#include "../util/HWRayTracingUtil.glsl"

layout(location = 0) rayPayloadInEXT RayPayload payload;

void main() {
    payload.hit = false;
    payload.t = -1.0;
    payload.hitPosition = vec3(0.0);
    payload.hitNormal = vec3(0.0, 1.0, 0.0);
    payload.uv = vec2(0.0);

    payload.material.baseColor = vec3(1.0, 0.0, 1.0);
    payload.material.roughness = 1.0;
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
