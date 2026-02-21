#ifndef MATERIAL_V
#define MATERIAL_V
struct MaterialData {
    vec3 albedo;
    float roughness;
    float metallic;
    uint isEmissive;

    uint useAlbedoTexture;
    uint useNormalTexture;
    uint useRoughnessTexture;
    uint useMetallicTexture;
    uint useAoTexture;
    uint useHeightTexture;

    uint albedoTexture;
    uint normalTexture;
    uint roughnessTexture;
    uint metallicTexture;
    uint aoTexture;
    uint heightTexture;
};

layout (set = MATERIAL_SET, binding = 0) readonly buffer Materials {
    MaterialData items[];
} materialBuffer;
#endif
