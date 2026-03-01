#ifndef MATERIAL_V
#define MATERIAL_V
struct MaterialData {
    vec3 albedo;
    float roughness;
    float metallic;
    float transmission;
    float thickness;
    float ior;
    uint isEmissive;

    uint useAlbedoTexture;
    uint useNormalTexture;
    uint useRoughnessTexture;
    uint useMetallicTexture;

    uint albedoTexture;
    uint normalTexture;
    uint roughnessTexture;
    uint metallicTexture;
};

layout (set = 0, binding = MATERIAL_SET) readonly buffer Materials {
    MaterialData items[];
} materialBuffer;
#endif
