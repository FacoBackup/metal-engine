#ifndef GBUFFER_UTIL_GLSL
#define GBUFFER_UTIL_GLSL

float compressRoughnessMetallic(float metallic, float roughness) {
    uint m = uint(clamp(metallic * 255.0, 0.0, 255.0));
    uint r = uint(clamp(roughness * 255.0, 0.0, 255.0));
    return float((m << 8) | r) / 65535.0;
}

void decompressRoughnessMetallic(float packedVal, out float metallic, out float roughness) {
    uint packed = uint(packedVal * 65535.0);
    metallic = float((packed >> 8) & 0xFFu) / 255.0;
    roughness = float(packed & 0xFFu) / 255.0;
}

#endif
