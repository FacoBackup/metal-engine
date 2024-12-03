#include <metal_stdlib>
using namespace metal;

// Vertex shader
vertex float4 vertex_main(const device float4* vertexArray [[buffer(0)]],
                          uint vertexID [[vertex_id]]) {
    return vertexArray[vertexID];
}

// Fragment shader
fragment float4 fragment_main() {
    return float4(1.0, 0.0, 0.0, 1.0); // Flat red color
}
