#ifndef MATERIAL_V
#define MATERIAL_V
struct MaterialBufferInstance {
    vec3 albedo;
    float roughness;
    float metallic;
    uint albedoTexture;
    uint normalTexture;
    uint roughnessTexture;
    uint metallicTexture;
    uint aoTexture;
};

layout (set = MATERIAL_SET, binding = 0) uniform Materials {
    MaterialBufferInstance items[MAX_MATERIALS];
} materialBuffer;
#endif
