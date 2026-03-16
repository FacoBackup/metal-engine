#include "../GlobalDataBuffer.glsl"

layout (push_constant) uniform Push {
    mat4 model;
    vec4 color;
} push;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inWorldPos;

layout(location = 0) out vec4 outFragColor;

void main() {
    vec3 N = normalize(inNormal);
    vec3 L = normalize(globalData.cameraWorldPosition - inWorldPos);
    vec3 V = L; // In Blinn-Phong, often L=V if we use camera as light source
    vec3 H = normalize(L + V);

    float diffuse = max(dot(N, L), 0.0);
    float specular = pow(max(dot(N, H), 0.0), 32.0);

    vec3 ambient = 0.1 * push.color.rgb;
    vec3 color = ambient + push.color.rgb * diffuse + vec3(specular);

    outFragColor = vec4(color, push.color.a);
}
