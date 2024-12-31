struct Hit {
    vec3 hitPosition;
    vec3 voxelPosition;
    bool anyHit;
    float voxelSize;
    uint voxel;
    uint voxelBufferIndex;
    uint bufferIndex;
    uint matData1;
    uint matData2;
};

struct VoxelMaterialData {
    vec3 albedo;
    bool isEmissive;
    float roughness;
    float metallic;
    vec3 normal;
};

VoxelMaterialData unpackVoxel(in Hit hit) {
    VoxelMaterialData voxel;

    uint first = hit.matData1;
    uint second = hit.matData2;

    {
        voxel.roughness = (first & 0xFFu)/100.;
        voxel.metallic = (second & 0x7Fu) / 100.;
    }

    {
        uint rInt = (first >> 20u) & 0x3FFu;// 10 bits for r (mask: 0x3FF is 1023 in binary)
        uint gInt = (first >> 10u) & 0x3FFu;// 10 bits for g
        uint bInt = first & 0x3FFu;// 10 bits for b

        // Convert the quantized integers back to floats in the range [0, 1]
        float r = rInt / 1023.0f;
        float g = gInt / 1023.0f;
        float b = bInt / 1023.0f;

        // Scale back to the original [-1, 1] range
        r = r * 2.0f - 1.0f;
        g = g * 2.0f - 1.0f;
        b = b * 2.0f - 1.0f;

        voxel.isEmissive = ((first >> 8u) & 0x1u) == 1;
        voxel.albedo.r = r;
        voxel.albedo.g = g;
        voxel.albedo.b = b;
    }

    {
        uint compressedNormal = second >> 7;
        int quantX = int((compressedNormal >> 13u) & 0x1FFFu);
        int quantY = int(compressedNormal & 0x1FFFu);
        float octX = (quantX - 2048.0f) / 2047.0f;
        float octY = (quantY - 2048.0f) / 2047.0f;
        float z = 1.0f - abs(octX) - abs(octY);
        if (z < 0.0f) {
            octX = (octX + sign(octX)) * 0.5f;
            octY = (octY + sign(octY)) * 0.5f;
        }
        voxel.normal = normalize(vec3(octX, octY, z));
    }
    return voxel;
}
