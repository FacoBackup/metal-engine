#ifndef V_V
#define V_V
struct Volume {
    vec4 color;
    vec3 position;
    vec3 dataA;
    vec3 dataB; //For volumes: <density, scatteringAlbedo, phase function asymmetry>
    uint itemType;
};

layout(set = VOLUMES_SET, binding = 0) uniform Volumes {
    Volume items[MAX_LIGHT_VOLUMES];
} volumesBuffer;
#endif
