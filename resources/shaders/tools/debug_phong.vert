#include "../GlobalDataBuffer.glsl"

layout (push_constant) uniform Push {
    mat4 model;
    vec4 color;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec3 outWorldPos;

void main() {
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    outWorldPos = worldPos.xyz;
    outNormal = normalize(mat3(push.model) * inNormal);
    gl_Position = globalData.projView * worldPos;
}
