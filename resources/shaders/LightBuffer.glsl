#ifndef L_V
#define L_V
struct Light {
    vec4 color;
    vec3 position;
    vec3 dataA;
    vec3 dataB;
    uint itemType;
};

layout(set = LIGHT_SET, binding = 0) readonly buffer Lights {
    Light items[];
} lightBuffer;
#endif
