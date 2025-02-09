#ifndef L_V
#define L_V
struct LightVolume {
    vec4 color;
    vec3 position;
    vec3 dataA;
    vec3 dataB; //For volumes: <density, scatteringAlbedo, phase function asymmetry>
    uint itemType;
};

layout(set = LIGHT_VOLUME_SET, binding = 0) uniform Lights {
    LightVolume items[MAX_LIGHT_VOLUMES];
} lightVolumeBuffer;
#endif
