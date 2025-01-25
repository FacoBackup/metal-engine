#include "./GlobalDataBuffer.glsl"

layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D noisyInput;
layout(set = 2, binding = 0) uniform sampler2D gBufferNormal;
layout(set = 3, binding = 0) uniform sampler2D gBufferPosition;

layout(location = 0) out vec4 finalColor;

layout(push_constant) uniform Push {
    float  colorPhi, normalPhi, positionPhi, stepWidth;
} push;

void main() {
    if (!globalData.enabledDenoiser){
        finalColor = vec4(texture(noisyInput, texCoords).rgb, 1);
        return;
    }
    vec2 offset[25];
    offset[0] = vec2(-2, -2);
    offset[1] = vec2(-1, -2);
    offset[2] = vec2(0, -2);
    offset[3] = vec2(1, -2);
    offset[4] = vec2(2, -2);

    offset[5] = vec2(-2, -1);
    offset[6] = vec2(-1, -1);
    offset[7] = vec2(0, -1);
    offset[8] = vec2(1, -1);
    offset[9] = vec2(2, -1);

    offset[10] = vec2(-2, 0);
    offset[11] = vec2(-1, 0);
    offset[12] = vec2(0, 0);
    offset[13] = vec2(1, 0);
    offset[14] = vec2(2, 0);

    offset[15] = vec2(-2, 1);
    offset[16] = vec2(-1, 1);
    offset[17] = vec2(0, 1);
    offset[18] = vec2(1, 1);
    offset[19] = vec2(2, 1);

    offset[20] = vec2(-2, 2);
    offset[21] = vec2(-1, 2);
    offset[22] = vec2(0, 2);
    offset[23] = vec2(1, 2);
    offset[24] = vec2(2, 2);

    float kernel[25];
    kernel[0] = 1.0f/256.0f;
    kernel[1] = 1.0f/64.0f;
    kernel[2] = 3.0f/128.0f;
    kernel[3] = 1.0f/64.0f;
    kernel[4] = 1.0f/256.0f;

    kernel[5] = 1.0f/64.0f;
    kernel[6] = 1.0f/16.0f;
    kernel[7] = 3.0f/32.0f;
    kernel[8] = 1.0f/16.0f;
    kernel[9] = 1.0f/64.0f;

    kernel[10] = 3.0f/128.0f;
    kernel[11] = 3.0f/32.0f;
    kernel[12] = 9.0f/64.0f;
    kernel[13] = 3.0f/32.0f;
    kernel[14] = 3.0f/128.0f;

    kernel[15] = 1.0f/64.0f;
    kernel[16] = 1.0f/16.0f;
    kernel[17] = 3.0f/32.0f;
    kernel[18] = 1.0f/16.0f;
    kernel[19] = 1.0f/64.0f;

    kernel[20] = 1.0f/256.0f;
    kernel[21] = 1.0f/64.0f;
    kernel[22] = 3.0f/128.0f;
    kernel[23] = 1.0f/64.0f;
    kernel[24] = 1.0f/256.0f;

    vec3 sum = vec3(0.0);
    vec2 step = 1./textureSize(noisyInput, 0);// resolution
    vec3 cval = texture(noisyInput, texCoords).rgb;
    vec3 nval = texture(gBufferNormal, texCoords).rgb;
    vec3 pval = texture(gBufferPosition, texCoords).rgb;
    float cum_w = 0.0;
    for (int i = 0; i < 25; i++) {
        vec2 uv = texCoords + offset[i]*step*push.stepWidth;
        vec3 ctmp = texture(noisyInput, uv).rgb;
        vec3 t = cval - ctmp;
        float dist2 = dot(t, t);
        float c_w = min(exp(-(dist2)/push.colorPhi), 1.0);
        vec3 ntmp = texture(gBufferNormal, uv).rgb;
        t = nval - ntmp;
        dist2 = max(dot(t, t)/(push.stepWidth*push.stepWidth), 0.0);
        float n_w = min(exp(-(dist2)/push.normalPhi), 1.0);
        vec3 ptmp = texture(gBufferPosition, uv).rgb;
        t = pval - ptmp;
        dist2 = dot(t, t);
        float p_w = min(exp(-(dist2)/push.positionPhi), 1.0);
        float weight = c_w * n_w * p_w;
        sum += ctmp * weight * kernel[i];
        cum_w += weight*kernel[i];
    }
    finalColor = vec4(sum/cum_w, 1);
}