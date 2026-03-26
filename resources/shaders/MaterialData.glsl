#ifndef MATERIAL_DATA_GLSL
#define MATERIAL_DATA_GLSL
struct MaterialData {
    float transmission;
    float thickness;
    float ior;
    uint isEmissive;

    uint albedoTexture;
    uint roughnessTexture;
    uint metallicTexture;
};

layout (set = 0, binding = MATERIAL_DATA_SET) readonly buffer MaterialDatas {
    MaterialData items[];
} materialBuffer;
#endif
