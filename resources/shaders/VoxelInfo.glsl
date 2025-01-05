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
        voxel.roughness = ((first & 0x7FFFFFFFu) >> 24)/100.;
        voxel.metallic = .5;
    }

    {
        int r = (int(first) >> 16) & 0xFF;
        int g = (int(first) >> 8) & 0xFF;
        int b = int(first) & 0xFF;

        r = r * 2;
        b = b * 2;

        voxel.isEmissive = (first >> 31u) == 1;
        voxel.albedo = vec3(r/255., g/255., b/255.);
    }

    {
        voxel.normal = unpackNormal(second);
    }
    return voxel;
}
