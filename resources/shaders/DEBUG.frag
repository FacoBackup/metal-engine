layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout (location = 0) out vec4 albedoEmissive;
layout (location = 1) out vec4 roughnessMetallicAO;
layout (location = 2) out vec4 normal;
layout (location = 3) out vec4 depthIdUV;

void main () {
    albedoEmissive = vec4(1, 0, 1, 0);
    roughnessMetallicAO = vec4(1, 0, 1, 1);
    normal = vec4(inNormal, 1);
    depthIdUV = vec4(1, 1, inUV);
}