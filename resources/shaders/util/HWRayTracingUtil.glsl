#ifndef HW_RAY_TRACING_UTIL_GLSL
#define HW_RAY_TRACING_UTIL_GLSL
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
};

struct RayPayload {
    MaterialInfo material;
    vec3 hitPosition;
    vec3 hitNormal;
    vec2 uv;
    uint renderIndex;
    bool hit;
    float t;
};
#endif