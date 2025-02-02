struct LightVolume {
    vec3 color;
    vec3 position;
    vec3 minNormal;
    uint itemType;
    float radiusSize; // Density for volume
};

layout(set = LIGHT_VOLUME_SET, binding = 0) uniform Lights {
    LightVolume items[MAX_LIGHT_VOLUMES];
} lightVolumeBuffer;
