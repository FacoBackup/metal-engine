#ifndef MESHMETADATA_GLSL
#define MESHMETADATA_GLSL

struct MeshMetadata {
    uint renderIndex;
    uint materialIndex;
    uint padding0;
    uint padding1;
};

layout (set = 0, binding = MESH_METADATA_SET) readonly buffer MeshMetadatas {
    MeshMetadata items[];
} meshMetadataBuffer;

#endif
