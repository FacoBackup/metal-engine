#ifndef MATERIAL_DATA_GLSL
#define MATERIAL_DATA_GLSL
struct MaterialData {
    uint isEmissive;

    uint albedoTextureId;
    uint roughnessTextureId;
    uint metallicTextureId;
    uint normalTextureId;
    uint heightTextureId;
    float parallaxScale;
};

layout (set = 0, binding = MATERIAL_DATA_SET) readonly buffer MaterialDatas {
    MaterialData items[];
} materialBuffer;
#endif
