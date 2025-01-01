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
        int r = (int(first) >> 16) & 0x7F;
        int g = (int(first) >> 8) & 0xFF;
        int b = int(first) & 0x7F;

        r = r * 2;
        b = b * 2;

        voxel.isEmissive = ((first >> 8u) & 0x1u) == 1;
        voxel.albedo = vec3(r/255., g/255., b/255.);
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
