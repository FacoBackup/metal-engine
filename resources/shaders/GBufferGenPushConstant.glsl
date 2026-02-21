layout(push_constant) uniform Push {
    mat4 model;
    vec4 albedoEmissive;

    uint renderIndex;
    uint materialIndex;
    float roughnessFactor;
    float metallicFactor;

    float parallaxHeightScale;
    int parallaxLayers;
} push;