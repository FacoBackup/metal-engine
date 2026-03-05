#ifndef MESHMETADATA_GLSL
#define MESHMETADATA_GLSL
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

struct MeshMetadata {
    uint renderIndex;
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;

    vec3 albedo;
    float roughness;
    float metallic;
    float transmission;
    float thickness;
    float ior;
    uint isEmissive;

    uint useAlbedoTexture;
    uint useNormalTexture;
    uint useRoughnessTexture;
    uint useMetallicTexture;

    uint albedoTexture;
    uint normalTexture;
    uint roughnessTexture;
    uint metallicTexture;
};

layout (set = 0, binding = MESH_METADATA_SET) readonly buffer MeshMetadatas {
    MeshMetadata items[];
} meshMetadataBuffer;

#endif
