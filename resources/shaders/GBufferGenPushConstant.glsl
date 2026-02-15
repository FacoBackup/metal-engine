layout(push_constant) uniform Push {
    mat4 model;
    vec4 albedoEmissive;

    uint renderIndex;
    float roughnessFactor;
    float metallicFactor;

    uint useAlbedoTexture;
    uint useNormalTexture;
    uint useRoughnessTexture;
    uint useMetallicTexture;
    uint useHeightTexture;

    float parallaxHeightScale;
    int parallaxLayers;
} push;