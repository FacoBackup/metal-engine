struct Volume {
    vec3 albedo;
    vec3 position;
    vec3 dimensions;
    float density;
};

layout(set = VOLUMES_SET, binding = 0) uniform Volumes {
    Volume volumes[MAX_VOLUMES];
} volumesBuffer;
