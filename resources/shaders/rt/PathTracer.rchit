#extension GL_EXT_ray_tracing: require

#include "../util/HWRayTracingUtil.glsl"
#define MATERIAL_DATA_SET 8
#define PRIMITIVE_DATA_SET 9

layout (set = 0, binding = 10) uniform sampler2D textureArray[];

layout (location = 0) rayPayloadInEXT RayPayload payload;
hitAttributeEXT vec2 attribs; // Barycentric coordinates for the hit

#include "../MaterialData.glsl"
#include "../PrimitiveData.glsl"
#include "../MeshDataSampling.glsl"

void main() {
    uint primitiveIndex = gl_InstanceCustomIndexEXT;
    PrimitiveData primitive = primitiveBuffer.items[primitiveIndex];

    TriangleData tri = getTriangleData(primitive, gl_PrimitiveID);

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    vec3 normal = tri.n0 * barycentrics.x + tri.n1 * barycentrics.y + tri.n2 * barycentrics.z;
    vec2 uv = tri.uv0 * barycentrics.x + tri.uv1 * barycentrics.y + tri.uv2 * barycentrics.z;

    payload.hit = true;
    payload.t = gl_HitTEXT;
    payload.hitPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    payload.hitNormal = normalize(vec3(gl_ObjectToWorldEXT * vec4(normal, 0.0)));

    payload.renderIndex = primitive.renderIndex;

    vec3 baseColor = vec3(1.0);
    float roughness = 1.0;
    float metallic = 0.0;
    bool isEmissive = false;

    MaterialData material = materialBuffer.items[primitive.materialIndex];
    if (material.albedoTextureId != 0u) {
        baseColor = texture(textureArray[nonuniformEXT(material.albedoTextureId)], uv).rgb;
    }
    if (material.roughnessTextureId != 0u) {
        roughness = texture(textureArray[nonuniformEXT(material.roughnessTextureId)], uv).r;
    }
    if (material.metallicTextureId != 0u) {
        metallic = texture(textureArray[nonuniformEXT(material.metallicTextureId)], uv).r;
    }
    isEmissive = material.isEmissive == 1u;

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
