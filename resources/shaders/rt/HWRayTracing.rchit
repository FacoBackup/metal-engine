#extension GL_EXT_ray_tracing: require
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "../util/HWRayTracingUtil.glsl"
#define MESH_METADATA_SET 7
#include "../MeshMetadata.glsl"

layout (set = 0, binding = 8) uniform sampler2D textureArray[];

layout (location = 0) rayPayloadInEXT RayPayload payload;
hitAttributeEXT vec2 attribs; // Barycentric coordinates for the hit

struct VertexData {
    float vx, vy, vz;
    float nx, ny, nz;
    float u, v;
};

layout(buffer_reference, std430) readonly buffer Vertices {
    VertexData vertices[];
};

layout(buffer_reference, std430) readonly buffer Indices {
    uint indices[];
};

void main() {
    uint metadataIndex = gl_InstanceCustomIndexEXT;
    MeshMetadata metadata = meshMetadataBuffer.items[metadataIndex];

    Vertices vBuffer = Vertices(metadata.vertexBufferAddress);
    Indices iBuffer = Indices(metadata.indexBufferAddress);

    uint i0 = iBuffer.indices[3 * gl_PrimitiveID];
    uint i1 = iBuffer.indices[3 * gl_PrimitiveID + 1];
    uint i2 = iBuffer.indices[3 * gl_PrimitiveID + 2];

    VertexData v0 = vBuffer.vertices[i0];
    VertexData v1 = vBuffer.vertices[i1];
    VertexData v2 = vBuffer.vertices[i2];

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    
    vec3 n0 = vec3(v0.nx, v0.ny, v0.nz);
    vec3 n1 = vec3(v1.nx, v1.ny, v1.nz);
    vec3 n2 = vec3(v2.nx, v2.ny, v2.nz);
    
    vec2 uv0 = vec2(v0.u, v0.v);
    vec2 uv1 = vec2(v1.u, v1.v);
    vec2 uv2 = vec2(v2.u, v2.v);

    vec3 normal = n0 * barycentrics.x + n1 * barycentrics.y + n2 * barycentrics.z;
    vec2 uv = uv0 * barycentrics.x + uv1 * barycentrics.y + uv2 * barycentrics.z;

    payload.hit = true;
    payload.t = gl_HitTEXT;
    payload.hitPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    payload.hitNormal = normalize(vec3(gl_ObjectToWorldEXT * vec4(normal, 0.0)));
    payload.uv = uv;

    uint matIndex = metadata.materialIndex;
    payload.renderIndex = metadata.renderIndex;

    vec3 baseColor = vec3(1.0);
    float roughness = 1.0;
    float metallic = 0.0;
    float transmission = 0.0;
    float thickness = 0.0;
    float ior = 1.45;
    bool isEmissive = false;

    if (matIndex != 0u) { 
        if (metadata.albedoTexture != 0u) {
            baseColor = texture(textureArray[nonuniformEXT(metadata.albedoTexture)], uv).rgb;
        } else {
            baseColor = metadata.albedo;
        }
        if (metadata.roughnessTexture != 0u) {
            roughness = texture(textureArray[nonuniformEXT(metadata.roughnessTexture)], uv).r;
        } else {
            roughness = metadata.roughness;
        }
        if (metadata.metallicTexture != 0u) {
            metallic = texture(textureArray[nonuniformEXT(metadata.metallicTexture)], uv).r;
        } else {
            metallic = metadata.metallic;
        }
        transmission = metadata.transmission;
        thickness = metadata.thickness;
        ior = metadata.ior;
        isEmissive = metadata.isEmissive == 1u;
    }

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
