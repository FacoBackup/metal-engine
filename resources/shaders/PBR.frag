layout (location = 0) in vec2 texCoords;

layout (set = 0, binding = 0) uniform sampler2D gBufferAlbedoEmissive;
layout (set = 0, binding = 1) uniform sampler2D gBufferRoughnessMetallic;
layout (set = 0, binding = 2) uniform sampler2D gBufferRenderIndexDepth;
layout (set = 0, binding = 3) uniform sampler2D gBufferNormal;

layout (location = 0) out vec4 finalColor;

void main() {

    finalColor = vec4(texture(gBufferAlbedoEmissive, texCoords).rgb, 1);
}
