layout(location = 0) in vec2 texCoords;

layout(push_constant) uniform Push {
    bool motionBlurEnabled;
    float motionBlurVelocityScale;
    int motionBlurMaxSamples;
} motionBlur;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;
layout(set = 0, binding = 1) uniform sampler2D depthBuffer;
layout(set = 0, binding = 2) uniform sampler2D velocityBuffer;

layout(location = 0) out vec4 finalColor;

void main() {
    vec3 color = texture(sceneColor, texCoords).rgb;

    if (!motionBlur.motionBlurEnabled) {
        finalColor = vec4(color, 1.0);
        return;
    }

    vec2 velocity = texture(velocityBuffer, texCoords).rg * motionBlur.motionBlurVelocityScale;

    // A simple multi-sample motion blur
    vec3 blurredColor = color;
    int samples = clamp(motionBlur.motionBlurMaxSamples, 1, 64);
    for (int i = 1; i < samples; ++i) {
        vec2 offset = velocity * (float(i) / float(samples - 1) - 0.5);
        blurredColor += texture(sceneColor, texCoords + offset).rgb;
    }

    finalColor = vec4(blurredColor / float(samples), 1.0);
}
