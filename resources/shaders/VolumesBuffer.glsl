#ifndef V_V
#define V_V
struct VolumeInstance {
    vec4 color;
    vec3 position;
    vec3 size;
    float density;
    float scatteringAlbedo;
    float phaseFunctionAsymmetry;
};

layout(set = VOLUMES_SET, binding = 0) uniform Volumes {
    VolumeInstance items[MAX_LIGHT_VOLUMES];
} volumesBuffer;
#endif
