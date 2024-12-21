#define LIT 0
#define ALBEDO 1
#define NORMAL 2
#define ROUGHNESS 3
#define METALLIC 4
#define AO 5
#define RANDOM 6

layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D gBufferAlbedoEmissive;
layout(set = 1, binding = 1) uniform sampler2D gBufferRoughnessMetallicAO;
layout(set = 1, binding = 2) uniform sampler2D gBufferNormal;
layout(set = 1, binding = 3) uniform sampler2D gBufferDepthIdUV;

layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 view;
    mat4 proj;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
} globalData;

layout(push_constant) uniform Push {
    int mode;
} push;

layout(location = 0) out vec4 finalColor;

void main() {
    if (push.mode == ALBEDO){
        finalColor = vec4(texture(gBufferAlbedoEmissive, texCoords).rgb, 1);
    } else if (push.mode == NORMAL){
        finalColor = vec4(texture(gBufferNormal, texCoords).rgb, 1);
    } else if (push.mode == ROUGHNESS){
        finalColor = vec4(vec3(texture(gBufferRoughnessMetallicAO, texCoords).r), 1);
    } else if (push.mode == METALLIC){
        finalColor = vec4(vec3(texture(gBufferRoughnessMetallicAO, texCoords).g), 1);
    } else if (push.mode == AO){
        finalColor = vec4(vec3(texture(gBufferRoughnessMetallicAO, texCoords).b), 1);
    } else {
        discard;
    }
}
