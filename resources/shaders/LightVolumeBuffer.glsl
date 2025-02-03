#ifndef L_V
#define L_V
struct LightVolume {
    vec3 color;
    vec3 position;
    vec3 dataA;
    vec3 dataB;// Density for volume
    uint itemType;
};
#endif

layout(set = LIGHT_VOLUME_SET, binding = 0) uniform Lights {
    LightVolume items[MAX_LIGHT_VOLUMES];
} lightVolumeBuffer;
