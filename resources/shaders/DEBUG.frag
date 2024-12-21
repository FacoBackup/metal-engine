layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout (location = 0) out vec4 outAlbedoEmissive;
layout (location = 1) out vec4 outRoughnessMetallicAO;
layout (location = 2) out vec4 outNormal;
layout (location = 3) out vec4 outDepthIdUV;

void main () {
    outAlbedoEmissive = vec4(1, 0, 0, 0);
    outRoughnessMetallicAO = vec4(0, 1, 0, 1);
    outNormal = vec4(inNormal, 1);
    outDepthIdUV = vec4(1, 1, inUV);
}