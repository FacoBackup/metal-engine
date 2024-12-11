#version 460

layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    mat4 invView;
    mat4 invProj;
    vec4 color;
} globalData;

layout(push_constant) uniform Push {
    vec3 color;
} push;

layout (location = 0) out vec4 finalColor;
layout(location = 0) in vec2 texCoord;

void main () {
    finalColor = vec4(globalData.color.rgb, 1);
}