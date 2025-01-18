
struct SurfaceInteraction {
    vec3 incomingRayDir;
    vec3 point;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    bool anyHit;

    vec3 voxelPosition;
    float voxelSize;
    uint voxel;
    uint voxelBufferIndex;
    uint bufferIndex;
    uint matData1;
    uint matData2;
};

struct MaterialInfo {
    vec3  baseColor;
    float subsurface;
    float roughness;
    float metallic;
    float specular;
    float specularTint;
    float clearcoat;
    float clearcoatGloss;
    float anisotropic;
    float sheen;
    float sheenTint;
    bool  isEmissive;
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

MaterialInfo unpackVoxel(in SurfaceInteraction hit) {
    MaterialInfo voxel;

    uint first = hit.matData1;
    uint second = hit.matData2;

    {
        voxel.baseColor.r = ((first >> 16u) & 0xFFu) / 255.f;
        voxel.baseColor.g = ((first >> 8u) & 0xFFu) / 255.f;
        voxel.baseColor.b = (first & 0xFFu) / 255.f;
        voxel.isEmissive = ((first >> 24u) & 0x1u) == 1;
    }

    hit.normal = unpackNormal(second);
    return voxel;
}
