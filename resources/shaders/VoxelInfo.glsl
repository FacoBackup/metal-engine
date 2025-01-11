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
    vec3 normal;
};

// THANKS TO https://www.shadertoy.com/view/llfcRl
vec3 unpackNormal(uint data) {
    uvec2 iuv = uvec2(data, data>>15u) & uvec2(32767u, 16383u);
    vec2 uv = vec2(iuv)*2.0/vec2(32767.0, 16383.0) - 1.0;

    uint is = (data>>31u)&1u;
    vec3 nor = vec3((is==0u)?1.0:-1.0, uv.xy);

    uint id = (data>>29u)&3u;
    if (id==0u) nor = nor.xyz;
    else if (id==1u) nor = nor.zxy;
    else nor = nor.yzx;

    return normalize(nor);
}

VoxelMaterialData unpackVoxel(in Hit hit) {
    VoxelMaterialData voxel;

    uint first = hit.matData1;
    uint second = hit.matData2;

    {
        voxel.albedo.r = ((first >> 16u) & 0xFFu) / 255.f;
        voxel.albedo.g = ((first >> 8u) & 0xFFu) / 255.f;
        voxel.albedo.b = (first & 0xFFu) / 255.f;
        voxel.albedo *= 4;
        voxel.isEmissive = ((first >> 24u) & 0x1u) == 1;
    }

    voxel.normal = unpackNormal(second);
    return voxel;
}
