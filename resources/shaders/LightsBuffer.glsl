struct Light {
    vec3 color;
    vec3 position;
    vec3 minNormal;
    vec3 max;
    uint lightType;
    float radiusSize;
};

layout(set = LIGHTS_SET, binding = 0) uniform Lights {
    Light lights[MAX_LIGHTS];
} lightsBuffer;
