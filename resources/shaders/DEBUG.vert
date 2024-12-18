layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 view;
    mat4 proj;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
} globalData;

layout(push_constant) uniform Push {
    mat4 model;
    mat4 data;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 position;

void main(){
    position = inPosition;
    gl_Position =  globalData.projView * push.model * vec4(inPosition, 1.0);
    gl_Position.y = -gl_Position.y;
}