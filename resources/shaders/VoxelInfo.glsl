struct Hit {
    bool anyHit;
    vec3 hitPosition;
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
        voxel.isEmissive = ((first >> 8u) & 0x1u) == 1;
        uint albedo = first >> 9;
        uint blue = albedo & 0x7Fu;
        uint green = (albedo >> 7) & 0xFFu;
        uint red = (albedo >> 16) & 0x7Fu;

        // Reconstruct color
        voxel.albedo.r = red * 2;
        voxel.albedo.g = green;
        voxel.albedo.b = blue * 2;
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
