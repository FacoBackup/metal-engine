#ifndef L_V
#define L_V
struct Light {
    uint triangleIndex;
    uint meshIndex;
    vec3 color;
};

layout(set = 0, binding = LIGHT_SET) readonly buffer Lights {
    Light items[];
} lightBuffer;
#endif
