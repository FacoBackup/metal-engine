layout(push_constant) uniform Push {
    mat4 model;
    mat4 previousModel;
    uint renderIndex;
    uint materialIndex;
    float parallaxHeightScale;
    int parallaxLayers;
} push;