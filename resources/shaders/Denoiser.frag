#include "./GlobalDataBuffer.glsl"

#ifdef DEBUG
#include "./DebugFlags.glsl"
#endif

layout(location = 0) in vec2 texCoords;

layout(set = 1, binding = 0) uniform sampler2D noisyInput;// Stores primitive ID in the A channel when denoiser is enabled
layout(set = 2, binding = 0) uniform sampler2D gBufferNormal;

layout(location = 0) out vec4 finalColor;

layout(push_constant) uniform Push {
    float  normalPhi, stepWidth;
} push;

// Gaussian function for weight calculation
float gaussian(float x, float sigma) {
    return exp(-0.5 * (x * x) / (sigma * sigma));
}

void main() {
    #ifdef DEBUG
    if (globalData.debugFlag != LIT && globalData.debugFlag != LIGHTING_ONLY){
        finalColor = vec4(texture(noisyInput, texCoords).rgb, 1);
        return;
    }
    #endif
    if (!globalData.enabledDenoiser){
        finalColor = vec4(texture(noisyInput, texCoords).rgb, 1);
        return;
    }

    vec2 u_resolution = textureSize(noisyInput, 0);
    vec3 centerNormal = texture(gBufferNormal, texCoords).xyz;
    vec4 centerColorAndId = texture(noisyInput, texCoords);// Color and ID at the current fragment
    float centerId = centerColorAndId.a;

    float sigmaSpatial = push.stepWidth / u_resolution.x;// Spatial Gaussian spread

    float totalWeight = 0.0;
    for (int y = -int(push.stepWidth); y <= int(push.stepWidth); y++) {
        for (int x = -int(push.stepWidth); x <= int(push.stepWidth); x++) {
            vec2 offset = vec2(float(x), float(y)) / u_resolution;
            vec2 sampleUV = texCoords + offset;

            vec4 sampleColorAndId = texture(noisyInput, sampleUV);
            vec3 sampleColor = sampleColorAndId.rgb;
            float sampleId = sampleColorAndId.a;

            vec3 sampleNormal = texture(gBufferNormal, sampleUV).xyz;

            // Check if the primitive ID matches
            if (abs(sampleId - centerId) > 0.001) {
                continue;
            }

            // Compute spatial and range weights
            float spatialWeight = gaussian(length(offset), sigmaSpatial);
            float rangeWeight = gaussian(length(sampleNormal - centerNormal), push.normalPhi);

            float weight = spatialWeight * rangeWeight;

            finalColor.rgb += sampleColor * weight;
            totalWeight += weight;
        }
    }

    finalColor.rgb /= totalWeight;
    finalColor = vec4(finalColor.rgb, 1);
}