#ifndef L_V
#define L_V
struct LightInstance {
    vec4 color;
    vec3 position;
    vec3 planeNormal;
    vec3 scale;
    uint itemType;
};

layout(set = LIGHT_VOLUME_SET, binding = 0) uniform Lights {
    LightInstance items[MAX_LIGHT_VOLUMES];
} lightsBuffer;
#endif
