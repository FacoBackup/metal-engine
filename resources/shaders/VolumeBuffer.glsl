#ifndef VOLUME_V
#define VOLUME_V
struct Volume {
    vec4 color;
    vec3 position;
    vec3 dataA;
    vec3 dataB; //For volumes: <density, scatteringAlbedo, phase function asymmetry>
};

layout(set = VOLUME_SET, binding = 0) uniform Volumes {
    Volume items[MAX_VOLUMES];
} volumesBuffer;
#endif
