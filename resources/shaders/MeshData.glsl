#ifndef MESHMETADATA_GLSL
#define MESHMETADATA_GLSL
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

struct PrimitiveData {
    uint materialIndex;
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
};

layout (set = 0, binding = PRIMITIVE_DATA_SET) readonly buffer PrimitiveDatas {
    PrimitiveData items[];
} primitiveBuffer;

#endif
