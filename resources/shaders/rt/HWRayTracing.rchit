#extension GL_EXT_ray_tracing: require

#include "../util/HWRayTracingUtil.glsl"
#define MESH_METADATA_SET 7

layout (set = 0, binding = 8) uniform sampler2D textureArray[];

layout (location = 0) rayPayloadInEXT RayPayload payload;
hitAttributeEXT vec2 attribs; // Barycentric coordinates for the hit

#include "../MeshMetadata.glsl"
#include "../MeshDataSampling.glsl"
#include "../util/GridUtil.glsl"

void main() {
    uint metadataIndex = gl_InstanceCustomIndexEXT;
    MeshMetadata metadata = meshMetadataBuffer.items[metadataIndex];

    TriangleData tri = getTriangleData(metadata, gl_PrimitiveID);

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    
    vec3 normal = tri.n0 * barycentrics.x + tri.n1 * barycentrics.y + tri.n2 * barycentrics.z;
    vec2 uv = tri.uv0 * barycentrics.x + tri.uv1 * barycentrics.y + tri.uv2 * barycentrics.z;

    payload.hit = true;
    payload.t = gl_HitTEXT;
    payload.hitPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    payload.hitNormal = normalize(vec3(gl_ObjectToWorldEXT * vec4(normal, 0.0)));
    payload.uv = uv;

    payload.renderIndex = metadata.renderIndex;

    vec3 baseColor = vec3(1.0);
    float roughness = 1.0;
    float metallic = 0.0;
    float transmission = 0.0;
    float thickness = 0.0;
    float ior = 1.45;
    bool isEmissive = false;

    payload.alpha = 1.0;
    if (metadata.albedoTexture != 0u) {
        vec4 alb = texture(textureArray[nonuniformEXT(metadata.albedoTexture)], uv);
        payload.alpha = alb.a;
        baseColor = alb.rgb;
    } else {
        baseColor = getDebugGrid(payload.hitPosition).baseColor;
    }
    if (metadata.roughnessTexture != 0u) {
        roughness = texture(textureArray[nonuniformEXT(metadata.roughnessTexture)], uv).r;
    } else {
        roughness = getDebugGrid(payload.hitPosition).roughness;
    }
    if (metadata.metallicTexture != 0u) {
        metallic = texture(textureArray[nonuniformEXT(metadata.metallicTexture)], uv).r;
    } else {
        metallic = getDebugGrid(payload.hitPosition).metallic;
    }
    transmission = metadata.transmission;
    thickness = metadata.thickness;
    ior = metadata.ior;
    isEmissive = metadata.isEmissive == 1u;

    payload.material.baseColor = baseColor;
    payload.material.roughness = max(roughness, 0.015);
    payload.material.metallic = metallic;
    payload.material.transmission = transmission;
    payload.material.thickness = thickness;
    payload.material.ior = ior;
    payload.material.subsurface = 0.0;
    payload.material.specular = 0.0;
    payload.material.specularTint = 0.0;
    payload.material.clearcoat = 0.0;
    payload.material.clearcoatGloss = 1.0;
    payload.material.anisotropic = 0.0;
    payload.material.sheen = 0.0;
    payload.material.sheenTint = 0.0;
    payload.material.isEmissive = isEmissive;
}
