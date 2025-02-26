#ifndef RT_STRUCTS
#define RT_STRUCTS

struct MaterialInfo {
    vec3  baseColor;
    float subsurface;
    float roughness;
    float metallic;
    float specular;
    float specularTint;
    float clearcoat;
    float clearcoatGloss;
    float anisotropic;
    float sheen;
    float sheenTint;
    bool  isEmissive;
};

struct SurfaceInteraction {
    vec3 incomingRayDir;
    vec3 point;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    bool anyHit;

    vec3 voxelPosition;
    float voxelSize;
    uint voxel;
    uint voxelBufferIndex;
    uint bufferIndex;
    uint matData1;
    uint matData2;
};

struct HitData {
    float closestT;
    vec3 hitNormal;
    vec2 hitUV;
    vec3 hitPosition;
    bool didHit;
    // TopLevelAS "id"; Refers to the primitive's unique id
    uint hitId;
    uint triangleTestCount;
    uint boxTestCount;
};

// --------------- RT TRIANGLE ---------------
struct RTTriangle {
    vec3 v1;
    vec3 v2;
    vec3 v3;

    vec3 n1;
    vec3 n2;
    vec3 n3;

    vec2 uv1;
    vec2 uv2;
    vec2 uv3;
};

layout(set = 1, binding = 0) uniform Triangles {
    RTTriangle items[MAX_RT_TRIANGLES];
} trianglesBuffer;

// --------------- BOTTOM LEVEL ---------------
struct BottomLevelAS {
    vec3 boundsMin;
    vec3 boundsMax;
    /**
    * When TriangleCount is negative (or not set), StartIndex is the index of the first child.
    * When positive, it is the index of the first triangle.
    */
    int startIndex;
    int triangleCount;
};
layout(set = 2, binding = 0) uniform BLAS {
    BottomLevelAS items[MAX_RT_BLAS];
} blasBuffer;

// --------------- TOP LEVEL ---------------
struct TopLevelAS {
    mat4 invTransform;
    uint nodeOffset;
    uint triangleOffset;
    uint id;
    uint materialId;
};
layout(set = 3, binding = 0) uniform TLAS {
    TopLevelAS items[MAX_RT_TLAS];
} tlasBuffer;

layout(set = 4, binding = 0) uniform Materials {
    MaterialInfo items[MAX_MATERIALS];
} materialBuffer;

layout(set = 5, binding = 0, rgba32f) uniform image2D outputImage;

#define LIGHT_VOLUME_SET 6
#include "../LightVolumeBuffer.glsl"

layout(set = 7, binding = 0, rgba32f) uniform image2D currentPositions;

#endif