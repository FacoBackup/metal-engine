#include "./Atmosphere.glsl"
#include "./GlobalDataBuffer.glsl"
#include "./CreateRay.glsl"
#include "./DepthUtils.glsl"

layout(set = 1, binding = 0) uniform sampler2D gBufferDepthIdUV;

layout(location = 0) out vec4 finalColor;
layout(location = 0) in vec2 texCoords;

void main(){
    float depthData = getLogDepth(texCoords, gBufferDepthIdUV, globalData.logDepthFC);
    if(depthData == 1){
        vec3 dir = createRay(texCoords, globalData.invProj, globalData.invView);
        finalColor = vec4(calculate_sky_luminance_rgb(normalize(globalData.sunPosition), dir, 2.0f) * 0.05f, 1.0f);
        return;
    }
    discard;
}
