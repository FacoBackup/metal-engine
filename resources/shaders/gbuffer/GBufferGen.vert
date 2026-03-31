#include "../GlobalDataBuffer.glsl"
#include "../gbuffer/GBufferGenPushConstant.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outLocalPosition;
layout(location = 4) out vec4 outCurrPos;
layout(location = 5) out vec4 outPrevPos;


void main(){
    outNormal =  mat3(transpose(inverse(push.model))) * inNormal;
    outUV = inUV;
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    outPosition = worldPos.rgb;
    outLocalPosition = inPosition;
    outCurrPos = globalData.projView * worldPos;
    outPrevPos = globalData.previousProjView * push.previousModel * vec4(inPosition, 1.0);
    gl_Position =  outCurrPos;
}