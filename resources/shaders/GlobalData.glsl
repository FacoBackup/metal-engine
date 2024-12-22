layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 view;
    mat4 proj;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
    vec3 sunColor;
    vec3 sunPosition;
} globalData;
