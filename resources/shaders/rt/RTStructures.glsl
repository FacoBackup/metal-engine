#ifndef RT_STRUCTS
#define RT_STRUCTS

#define MAX_RT_TRIANGLES 10000000
#define MAX_RT_BLAS 1000000
#define MAX_RT_TLAS 10000

struct HitData {
    float closestT;
    vec3 hitNormal;
    vec2 hitUV;
    vec3 hitPosition;
    bool didHit;
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
// stores max position of bounding box and the triangle's offset stored on the triangles buffer
    vec4 maxOffset;
// stores min position of bounding box and the triangle count
    vec4 minCount;
};
layout(set = 2, binding = 0) uniform BLAS {
    BottomLevelAS items[MAX_RT_BLAS];
} blasBuffer;

// --------------- TOP LEVEL ---------------
struct TopLevelAS {
    mat4 transform;
    uint bottomLevelASOffset;
};
layout(set = 3, binding = 0) uniform TLAS {
    TopLevelAS items[MAX_RT_TLAS];
} tlasBuffer;
#endif