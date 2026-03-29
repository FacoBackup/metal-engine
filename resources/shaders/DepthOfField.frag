layout(location = 0) in vec2 texCoords;

layout(push_constant) uniform Push {
    float focusDistance;
    float aperture;
    float focalLength;
    uint enabled;
} dof;

layout(set = 0, binding = 0, rgba32f) uniform readonly image2D sceneColor;
layout(set = 0, binding = 1) uniform sampler2D gBufferRenderIndexDepth;

layout(location = 0) out vec4 finalColor;

void main() {
    ivec2 size = imageSize(sceneColor);
    ivec2 iTexCoords = ivec2(texCoords * vec2(size));

    vec4 color = imageLoad(sceneColor, iTexCoords);
    if (dof.enabled == 0) {
        finalColor = color;
        return;
    }

    float depth = texture(gBufferRenderIndexDepth, texCoords).y;

    // Circle of Confusion (CoC) calculation
    // CoC = |A * (f * (D - d)) / (D * (d - f))|
    // where A is aperture, f is focal length, D is focus distance, d is depth

    float f = dof.focalLength;
    float D = dof.focusDistance;
    float d = depth;
    float A = dof.aperture;

    float coc = abs(A * (f * (D - d)) / (D * (d - f)));
    coc = clamp(coc, 0.0, 0.05); // Limit max blur

    if (coc < 0.0001) {
        finalColor = color;
        return;
    }

    // Simple blur based on CoC
    vec3 blurColor = vec3(0.0);
    float totalWeight = 0.0;
    int samples = 8;

    for (int i = -samples; i <= samples; i++) {
        for (int j = -samples; j <= samples; j++) {
            vec2 offset = vec2(i, j) * coc / float(samples);
            ivec2 sampleCoords = ivec2((texCoords + offset) * vec2(size));
            sampleCoords = clamp(sampleCoords, ivec2(0), size - ivec2(1));

            float weight = exp(-(float(i*i + j*j)) / (2.0 * float(samples * samples)));
            blurColor += imageLoad(sceneColor, sampleCoords).rgb * weight;
            totalWeight += weight;
        }
    }

    finalColor = vec4(blurColor / totalWeight, color.a);
}
