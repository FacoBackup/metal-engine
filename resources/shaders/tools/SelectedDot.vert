#include "../GlobalDataBuffer.glsl"
#include "../GBufferGenPushConstant.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) flat out uint renderIndex;


void main() {
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    renderIndex = push.renderIndex + 1;
    gl_Position = globalData.projView * worldPos;
}
