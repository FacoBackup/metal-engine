#ifndef MESH_DATA_SAMPLING
#define MESH_DATA_SAMPLING

#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64: require
#extension GL_EXT_buffer_reference2: require

struct VertexData {
    float vx, vy, vz;
    float nx, ny, nz;
    float u, v;
};

layout (buffer_reference, std430) readonly buffer Vertices {
    VertexData items[];
};

layout (buffer_reference, std430) readonly buffer Indices {
    uint items[];
};

struct TriangleData {
    vec3 v0, v1, v2;
    vec3 n0, n1, n2;
    vec2 uv0, uv1, uv2;
};

TriangleData getTriangleData(inout PrimitiveData metadata, uint triangleId) {

    Vertices vBuffer = Vertices(metadata.vertexBufferAddress);
    Indices iBuffer = Indices(metadata.indexBufferAddress);

    uint i0 = iBuffer.items[3 * triangleId];
    uint i1 = iBuffer.items[3 * triangleId + 1];
    uint i2 = iBuffer.items[3 * triangleId + 2];

    VertexData vd0 = vBuffer.items[i0];
    VertexData vd1 = vBuffer.items[i1];
    VertexData vd2 = vBuffer.items[i2];

    TriangleData data;
    data.v0 = vec3(vd0.vx, vd0.vy, vd0.vz);
    data.v1 = vec3(vd1.vx, vd1.vy, vd1.vz);
    data.v2 = vec3(vd2.vx, vd2.vy, vd2.vz);

    data.n0 = vec3(vd0.nx, vd0.ny, vd0.nz);
    data.n1 = vec3(vd1.nx, vd1.ny, vd1.nz);
    data.n2 = vec3(vd2.nx, vd2.ny, vd2.nz);

    data.uv0 = vec2(vd0.u, vd0.v);
    data.uv1 = vec2(vd1.u, vd1.v);
    data.uv2 = vec2(vd2.u, vd2.v);

    return data;
}
#endif