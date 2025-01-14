float compressRoughnessMetallic(float metallic, float roughness) {
    metallic = clamp(metallic, 0.0, 1.0);
    roughness = clamp(roughness, 0.0, 1.0);
    float metallicInt = floor(metallic * 65535.0);
    float roughnessInt = floor(roughness * 65535.0);
    return metallicInt * 65536.0 + roughnessInt;
}

// X = Metallic ; Y = Roughness
vec2 decompressRoughnessMetallic(float packedValue) {
    // Extract the two components
    float metallic = floor(packedValue / 65536.0) / 65535.0;
    float roughness = mod(packedValue, 65536.0) / 65535.0;
    return vec2(metallic, roughness);
}