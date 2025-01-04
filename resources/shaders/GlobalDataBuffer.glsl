layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
    vec3 sunColor;
    vec3 sunPosition;
    float logDepthFC;
    uint lightsQuantity;
    bool enabledSun;

// POST PROCESSING
    float distortionIntensity;
    float chromaticAberrationIntensity;
    bool distortionEnabled;
    bool chromaticAberrationEnabled;
    bool vignetteEnabled;
    float vignetteStrength;

// GI
    uint giBounces;
    uint giSamplesPerPixel;

    uint debugFlag;
} globalData;
