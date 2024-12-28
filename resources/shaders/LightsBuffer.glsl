struct Light {
    vec3 color;
    vec3 position;
    float innerRadius;
    float outerRadius;
    float radius;
};

layout(set = LIGHTS_SET, binding = LIGHTS_BINDING) uniform Lights {
    Light lights[MAX_LIGHTS];
} lightsBuffer;

