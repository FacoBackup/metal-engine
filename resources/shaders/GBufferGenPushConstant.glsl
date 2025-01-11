layout(push_constant) uniform Push {
    mat4 model;
    vec4 albedoEmissive;
    uint renderIndex;
    float roughnessFactor;
    float metallicFactor;

    bool useAlbedoTexture;
    bool useNormalTexture;
    bool useRoughnessTexture;
    bool useMetallicTexture;
    bool useAOTexture;
    bool useHeightTexture;
    float parallaxHeightScale;
    int parallaxLayers;
} push;