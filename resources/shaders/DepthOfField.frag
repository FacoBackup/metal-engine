layout(location = 0) in vec2 texCoords;

layout(push_constant) uniform Push {
    float focusDistance;
    float aperture;
    float focalLength;
    uint enabled;
} dof;

layout(set = 0, binding = 0, rgba32f) uniform readonly image2D sceneColor;
layout(set = 0, binding = 1) uniform sampler2D gBufferRenderIndexDepth;

// Assuming standard GlobalDataBlock for projection matrix access if needed
layout (set = 0, binding = 2, std140) uniform GlobalDataBlock {
    mat4 projView;
    mat4 previousProjView;
    mat4 invView;
    mat4 invProj;
    mat4 invProjView;
} globalData;

layout(location = 0) out vec4 finalColor;

float linearizeDepth(float d) {
    // d is in [0, 1] (standard Vulkan depth)
    // We use the inverse projection matrix to get view-space depth
    vec4 clip = vec4(texCoords.x * 2.0 - 1.0, texCoords.y * 2.0 - 1.0, d, 1.0);
    vec4 view = globalData.invProj * clip;
    return abs(view.z / view.w);
}

void main() {
    ivec2 size = imageSize(sceneColor);
    ivec2 iTexCoords = ivec2(texCoords * vec2(size));

    vec4 color = imageLoad(sceneColor, iTexCoords);
    if (dof.enabled == 0) {
        finalColor = color;
        return;
    }

    float rawDepth = texture(gBufferRenderIndexDepth, texCoords).y;
    if (rawDepth == 0.0) {
        finalColor = color;
        return;
    }

    float d = linearizeDepth(rawDepth);
    
    // Circle of Confusion (CoC) calculation
    // CoC = |A * (f * (D - d)) / (d * (D - f))|
    // A: aperture, f: focal length, D: focus distance, d: linear depth
    
    float f = dof.focalLength;
    float D = dof.focusDistance;
    float A = dof.aperture;

    float coc = abs(A * (f * (D - d)) / (d * (D - f)));
    coc = clamp(coc, 0.0, 0.02); // Limit max blur

    if (coc < 0.001) {
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

            float distSq = float(i*i + j*j);
            float weight = exp(-distSq / (2.0 * float(samples * samples)));
            
            blurColor += imageLoad(sceneColor, sampleCoords).rgb * weight;
            totalWeight += weight;
        }
    }

    finalColor = vec4(blurColor / totalWeight, color.a);
}
