float compressRoughnessMetallic(float metallic, float roughness) {
    roughness = clamp(roughness, 0, .99);
    metallic = clamp(metallic, 0, 1);
    return floor(metallic * 65535.0 + 0.5) + (floor(roughness * 65535.0 + 0.5) / 65536.0);
}

// X = Metallic ; Y = Roughness
vec2 decompressRoughnessMetallic(float packedValue) {
    float metallic = floor(packedValue);
    float roughness = fract(packedValue) * 65536.0;
    return vec2(metallic / 65535.0, roughness / 65535.0);
}