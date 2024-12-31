#include "./GlobalDataBuffer.glsl"

layout(push_constant) uniform Push {
    mat4 model;
    int renderIndex;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out flat uint outRenderingIndex;

void main(){
    outRenderingIndex = push.renderIndex;
    outPosition = inPosition;
    outNormal = inNormal;
    outUV = inUV;
    gl_Position =  globalData.projView * push.model * vec4(outPosition, 1.0);
}