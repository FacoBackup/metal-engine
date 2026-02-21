#include "../GlobalDataBuffer.glsl"

layout (push_constant) uniform Push {
    mat4 model;
    vec4 selectionColor;
    uint renderIndex;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

void main() {
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    gl_Position = globalData.projView * worldPos;
}
