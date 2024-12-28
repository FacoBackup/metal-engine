#define HALF_TILE_SIZE 16
#define MAX_VOXEL_SIZE 2000000
#define COUNT 32.

#define DEFINE_OCTREE_BUFFER(B, M, N) \
layout(std430, set = 1, binding = B) readonly buffer M { \
    uint voxels[MAX_VOXEL_SIZE]; \
 } N;

layout(set = 1, binding = 0) uniform TileInfo {
    vec4 tileCenterValid[9];
} tileInfo;

DEFINE_OCTREE_BUFFER(1, V1, voxelBuffer1)
DEFINE_OCTREE_BUFFER(2, V2, voxelBuffer2)
DEFINE_OCTREE_BUFFER(3, V3, voxelBuffer3)
DEFINE_OCTREE_BUFFER(4, V4, voxelBuffer4)
DEFINE_OCTREE_BUFFER(5, V5, voxelBuffer5)
DEFINE_OCTREE_BUFFER(6, V6, voxelBuffer6)
DEFINE_OCTREE_BUFFER(7, V7, voxelBuffer7)
DEFINE_OCTREE_BUFFER(8, V8, voxelBuffer8)
DEFINE_OCTREE_BUFFER(9, V9, voxelBuffer9)

struct Hit {
    bool hasAny;
    vec3 hitPosition;
    uint voxel;
    uint voxelBufferIndex;
};
struct Ray { vec3 o, d, invDir; };
struct Stack { uint index;vec3 center;float scale; };

bool intersect(inout vec3 boxMin, inout vec3 boxMax, inout Ray r) {
    vec3 t1 = (boxMin - r.o) * r.invDir;
    vec3 t2 = (boxMax - r.o) * r.invDir;

    vec3 tMin = min(t1, t2);
    vec3 tMax = max(t1, t2);

    float tEnter = max(max(tMin.x, tMin.y), tMin.z);
    float tExit = min(min(tMax.x, tMax.y), tMax.z);

    return tEnter <= tExit && tExit > 0.0;
}

bool intersectWithDistance(inout vec3 boxMin, inout vec3 boxMax, inout Ray r, out float entryDist) {
    vec3 t1 = (boxMin - r.o) * r.invDir;
    vec3 t2 = (boxMax - r.o) * r.invDir;

    vec3 tMin = min(t1, t2);
    vec3 tMax = max(t1, t2);

    entryDist = max(max(tMin.x, tMin.y), tMin.z);// Closest entry point along the ray
    float exitDist = min(min(tMax.x, tMax.y), tMax.z);// Furthest exit point along the ray

    return entryDist <= exitDist && exitDist > 0.0;// Ensure valid intersection and that exit is in front of the ray origin
}

vec3 unpackColor(int color) {
    int rInt = (color >> 20) & 0x3FF;// 10 bits for r (mask: 0x3FF is 1023 in binary)
    int gInt = (color >> 10) & 0x3FF;// 10 bits for g
    int bInt = color & 0x3FF;// 10 bits for b

    // Convert the quantized integers back to floats in the range [0, 1]
    float r = rInt / 1023.0f;
    float g = gInt / 1023.0f;
    float b = bInt / 1023.0f;

    // Scale back to the original [-1, 1] range
    r = r * 2.0f - 1.0f;
    g = g * 2.0f - 1.0f;
    b = b * 2.0f - 1.0f;

    return vec3(r, g, b);
}

uint countSetBitsBefore(inout uint mask, inout uint childIndex) {
    uint maskBefore = mask & ((1u << childIndex) - 1u);
    return bitCount(maskBefore);
}

Hit trace(
in Ray ray,
uint bufferIndex
#ifdef DEBUG_VOXELS
, bool showRaySearchCount,
bool showRayTestCount,
inout vec3 finalColor
#endif
) {
    vec4 localTileInfo = tileInfo.tileCenterValid[bufferIndex - 1];
    if (localTileInfo.w == 0) { return Hit(false, vec3(0), 0, 0); }

    vec3 center = localTileInfo.xyz * HALF_TILE_SIZE * 2;
    float scale = HALF_TILE_SIZE;
    vec3 minBox = center - scale;
    vec3 maxBox = center + scale;
    float minDistance = 1e10;// Large initial value

    if (!intersect(minBox, maxBox, ray)) return Hit(false, vec3(0), 0, 0);
    Stack stack[10];
    scale *= 0.5f;
    stack[0] = Stack(0u, center, scale);

    uint index = 0u;
    int rayTestCount = 0;
    #ifdef DEBUG_VOXELS
    int searchCount = 0;
    #endif
    int stackPos = 1;
    Hit hitData = Hit(false, vec3(0), 0, 0);
    while (stackPos-- > 0) {
        #ifdef DEBUG_VOXELS
        if (showRaySearchCount){
            searchCount ++;
            finalColor.r = searchCount/COUNT;
        }
        #endif
        center = stack[stackPos].center;
        index = stack[stackPos].index;
        scale = stack[stackPos].scale;
        uint voxel_node;
        switch (bufferIndex){
            case 1:
            voxel_node = voxelBuffer1.voxels[index];
            break;
            case 2:
            voxel_node = voxelBuffer2.voxels[index];
            break;
            case 3:
            voxel_node = voxelBuffer3.voxels[index];
            break;
            case 4:
            voxel_node = voxelBuffer4.voxels[index];
            break;
            case 5:
            voxel_node = voxelBuffer5.voxels[index];
            break;
            case 6:
            voxel_node = voxelBuffer6.voxels[index];
            break;
            case 7:
            voxel_node = voxelBuffer7.voxels[index];
            break;
            case 8:
            voxel_node = voxelBuffer8.voxels[index];
            break;
            case 9:
            voxel_node = voxelBuffer9.voxels[index];
            break;
            default :
            return hitData;
        }
        uint childGroupIndex = (voxel_node >> 9) & 0x7FFFFFu;
        uint childMask =  (voxel_node & 0xFFu);
        bool isLeafGroup = ((voxel_node >> 8) & 0x1u) == 1u;

        for (uint i = 0u; i < 8u; ++i) {
            if ((childMask & (1u << i)) == 0u){
                continue;
            }
            vec3 offset = vec3(
            (i & 1u) != 0u ? 1.0 : -1.0,
            (i & 2u) != 0u ? 1.0 : -1.0,
            (i & 4u) != 0u ? 1.0 : -1.0
            );
            vec3 newCenter = center + scale * offset;
            vec3 minBox = newCenter - scale;
            vec3 maxBox = newCenter + scale;

            #ifdef DEBUG_VOXELS
            if (showRayTestCount){
                rayTestCount++;
                finalColor.g = rayTestCount/COUNT;
            }
            #endif

            float entryDist;
            if (!intersectWithDistance(minBox, maxBox, ray, entryDist)) {
                continue;
            }
            if (entryDist < minDistance) {
                if (isLeafGroup) {
                    hitData.hasAny = true;
                    hitData.hitPosition = newCenter;
                    hitData.voxel = voxel_node;
                    hitData.voxelBufferIndex = index;
                    minDistance = entryDist;
                } else {
                    stack[stackPos++] = Stack(childGroupIndex + countSetBitsBefore(childMask, i), newCenter, scale * 0.5f);
                }
            }
        }
    }
    return hitData;
}

Hit traceAllTiles(
in Ray ray
#ifdef DEBUG_VOXELS
, bool showRaySearchCount,
bool showRayTestCount,
inout vec3 colorData
#endif
){
    Hit hitData = trace(ray, 1
    #ifdef DEBUG_VOXELS
    , showRaySearchCount, showRayTestCount, colorData
    #endif
    );
    if (!hitData.hasAny){
        hitData = trace(ray, 2
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 3
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 4
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 5
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 6
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 7
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 8
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.hasAny){
        hitData = trace(ray, 9
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    return hitData;
} 