#include "../GlobalDataBuffer.glsl"
#include "../gbuffer/GBufferGenPushConstant.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

#ifdef DEBUG
vec3 randomColor(uint seed) {
    float hash = fract(sin(float(seed)) * 43758.5453);
    float r = fract(hash * 13.756);
    float g = fract(hash * 15.734);
    float b = fract(hash * 17.652);
    return vec3(r, g, b);
}
#endif

void main(){
    outNormal =  mat3(transpose(inverse(push.model))) * inNormal;
    outUV = inUV;
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    outPosition = worldPos.rgb;
    gl_Position =  globalData.projView * worldPos;

}