#ifndef MESHMETADATA_GLSL
#define MESHMETADATA_GLSL
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

struct MeshMetadata {
    uint renderIndex;
    uint materialIndex;
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
};

layout (set = 0, binding = MESH_METADATA_SET) readonly buffer MeshMetadatas {
    MeshMetadata items[];
} meshMetadataBuffer;

#endif
