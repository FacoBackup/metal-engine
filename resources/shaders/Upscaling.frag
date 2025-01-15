layout(location = 0) in vec2 texCoords;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;
layout(set = 1, binding = 0) uniform sampler2D gBufferPosition;

layout(location = 0) out vec4 finalColor;

layout(push_constant) uniform Push {
    float   upscaleFactor;
    vec2    targetImageSize;
} push;

//#define UPSCALE_FACTOR push.upscaleFactor
//
//float gaussianDistrib(float x, const float sigma) {
//    return (1.0/(2.0*PI*pow(sigma,2.0)))*exp(-x*x/(2.0*pow(sigma,2.0)));
//}
//
//vec4 upsampleAt(vec2 p) {
//    float totalWeight = 0.0;
//    vec4 color = vec4(0.0);
//
//    const float sigmaSpatial = 0.5;
//    const float sigmaRange = 0.1;
//
//    const int hw = 3;
//    for (int u = -hw; u <= hw; ++u) {
//        for (int v = -hw; v <= hw; ++v) {
//            vec2 q = UPSCALE_FACTOR*floor(p/UPSCALE_FACTOR) + vec2(0.5) + UPSCALE_FACTOR*vec2(u,v);
//
//            vec4 Sq = texelFetch(sceneColor, ivec2(q/UPSCALE_FACTOR), 0);
//
//            float wSpatial = gaussianDistrib(length((p-q)/UPSCALE_FACTOR), sigmaSpatial);
//            float pDepth = texelFetch(gBufferPosition, ivec2(p), 0).a;
//            float qDepth = texelFetch(gBufferPosition, ivec2(q), 0).a;
//            float wRange = gaussianDistrib(abs(pDepth-qDepth), sigmaRange);
//            totalWeight += wSpatial*wRange;
//            color += wSpatial*wRange*Sq;
//        }
//    }
//    return color / totalWeight;
//}

void main() {
    finalColor = texture(sceneColor, texCoords );
}