#extension GL_EXT_ray_tracing: require
#extension GL_EXT_nonuniform_qualifier: enable

#include "../util/HWRayTracingUtil.glsl"
#define MATERIAL_SET 9
#include "../MaterialBuffer.glsl"
#define MESH_METADATA_SET 10
#include "../MeshMetadata.glsl"

layout (set = 0, binding = 11) uniform sampler2D textureArray[];

layout (location = 0) rayPayloadInEXT RayPayload payload;
hitAttributeEXT vec2 attribs; // Barycentric coordinates for the hit

void main() {
    payload.hit = true;
    payload.t = gl_HitTEXT;
    payload.hitPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    payload.hitNormal = normalize(-gl_WorldRayDirectionEXT);

    // Use hit barycentric coords as a provisional UV. This will be replaced with proper vertex UV interpolation later.
    vec2 uv = attribs;

    uint metadataIndex = gl_InstanceCustomIndexEXT;
    MeshMetadata metadata = meshMetadataBuffer.items[metadataIndex];
    uint matIndex = metadata.materialIndex;
    payload.renderIndex = metadata.renderIndex;

    vec3 baseColor = vec3(1.0);
    float roughness = 1.0;
    float metallic = 0.0;
    bool isEmissive = false;

    if (matIndex != 0u) {
        MaterialData mat = materialBuffer.items[matIndex];
        if (mat.useAlbedoTexture == 1u) {
            baseColor = texture(textureArray[nonuniformEXT(mat.albedoTexture)], uv).rgb;
        } else {
            baseColor = mat.albedo;
        }
        if (mat.useRoughnessTexture == 1u) {
            roughness = texture(textureArray[nonuniformEXT(mat.roughnessTexture)], uv).r;
        } else {
            roughness = mat.roughness;
        }
        if (mat.useMetallicTexture == 1u) {
            metallic = texture(textureArray[nonuniformEXT(mat.metallicTexture)], uv).r;
        } else {
            metallic = mat.metallic;
        }
        isEmissive = mat.isEmissive == 1u;
    }

    payload.material.baseColor = baseColor;
    payload.material.roughness = max(roughness, 0.015);
    payload.material.metallic = metallic;
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
