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

// -------------------------
// Constants & Helper Types
// -------------------------
#define INF 1e20
#define EPSILON 1e-6

struct Ray {
    vec3 origin;
    vec3 dir;
    vec3 invDir;
};

struct BounceInfo {
    vec3 albedo;
    bool isEmissive;
    vec3 hitNormal;
    vec3 currentPosition;
    vec3 throughput;
    vec3 indirectLight;
};

struct HitData {
    vec3 hitNormal;
    vec3 incomingRayDir;
    vec3 tangent;
    vec3 binormal;
    vec3 hitPosition;
    vec2 hitUV;

    bool didHit;
    float closestT;
    // TopLevelAS "id"; Refers to the primitive's unique id
    uint hitId;
    uint triangleId;
    uint materialId;
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

    int materialId;
};

#include "../GlobalDataBuffer.glsl"

layout(set = 0, binding = 1) uniform Triangles {
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
layout(set = 0, binding = 2) uniform BLAS {
    BottomLevelAS items[MAX_RT_BLAS];
} blasBuffer;

// --------------- TOP LEVEL ---------------
struct TopLevelAS {
    mat4 invTransform;
    uint nodeOffset;
    uint triangleOffset;
    uint id;
    // -1 if no material is configured
    int materialId;
};
layout(set = 0, binding = 3) uniform TLAS {
    TopLevelAS items[MAX_RT_TLAS];
} tlasBuffer;

layout(set = 0, binding = 4) uniform Materials {
    MaterialInfo items[MAX_MATERIALS];
} materialBuffer;


layout(set = 0, binding = 5, rgba32f) uniform image2D outputImage;
layout(set = 0, binding = 6, rgba32f) uniform image2D currentPositions;
layout(set = 0, binding = 7, rgba32f) uniform image2D giSurfaceCacheCompute;

#define LIGHT_VOLUME_SET 1
#include "../LightVolumeBuffer.glsl"


#endif