#include "./VoxelInfo.glsl"

#define MAX_VOXEL_SIZE 10000000
#define COUNT 32.

#define DEFINE_OCTREE_BUFFER(B, M, N) \
layout(std430, set = 1, binding = B) readonly buffer M { \
    uint voxels[MAX_VOXEL_SIZE]; \
 } N;

layout(set = 1, binding = 0) uniform TileInfo {
    vec4 tileCenterValid[9];
    uint voxelBufferOffset[9];
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

uint countSetBitsBefore(inout uint mask, inout uint childIndex) {
    uint maskBefore = mask & ((1u << childIndex) - 1u);
    return bitCount(maskBefore);
}

uint getVoxel(uint index, uint bufferIndex){
    switch (bufferIndex){
        case 1:
        return voxelBuffer1.voxels[index];
        case 2:
        return voxelBuffer2.voxels[index];
        case 3:
        return voxelBuffer3.voxels[index];
        case 4:
        return voxelBuffer4.voxels[index];
        case 5:
        return voxelBuffer5.voxels[index];
        case 6:
        return voxelBuffer6.voxels[index];
        case 7:
        return voxelBuffer7.voxels[index];
        case 8:
        return voxelBuffer8.voxels[index];
        case 9:
        return voxelBuffer9.voxels[index];
        default :
        return 0;
    }
}

vec4 intersectRayAABB(in Ray ray, vec3 boxMin, vec3 boxMax) {
    vec3 tMin = (boxMin - ray.o) * ray.invDir;
    vec3 tMax = (boxMax - ray.o) * ray.invDir;

    // Swap tMin and tMax for negative directions
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    // Find the largest t1 and the smallest t2
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    // Check if the ray intersects the AABB
    if (tNear > tFar || tFar < 0.0) {
        return vec4(0);// No intersection
    }

    // Compute intersection point
    float t = tNear > 0.0 ? tNear : tFar;// Use tNear if it's in front
    return vec4(ray.o + t * ray.d, 1);
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
    uint matrialBufferOffset = tileInfo.voxelBufferOffset[bufferIndex - 1];
    if (localTileInfo.w == 0) { return Hit(vec3(0), vec3(0), false, 0, 0, 0, bufferIndex, 0, 0); }

    vec3 center = localTileInfo.xyz * TILE_SIZE ;
    float scale = TILE_SIZE / 2;
    vec3 minBox = center - scale;
    vec3 maxBox = center + scale;
    float minDistance = 1e10;// Large initial value

    if (!intersect(minBox, maxBox, ray)) return Hit(vec3(0), vec3(0), false, 0, 0, 0, bufferIndex, 0, 0);
    Stack stack[6];
    scale *= 0.5f;
    stack[0] = Stack(0u, center, scale);

    uint index = 0u;
    int rayTestCount = 0;
    #ifdef DEBUG_VOXELS
    int searchCount = 0;
    #endif
    int stackPos = 1;
    Hit hitData = Hit(vec3(0), vec3(0), false, 0, 0, 0, bufferIndex, 0, 0);
    uint hitIndex;
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
        uint voxel_node = getVoxel(index, bufferIndex);

        bool isLeafGroup = ((voxel_node & 0x1u) == 1u);
        uint childMask = (voxel_node >> 1) & 0xFFu;
        uint childGroupIndex = (voxel_node >> 9) & 0x7FFFFFu;

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
                    hitData.anyHit = true;
                    hitData.voxelPosition = newCenter;
                    hitData.voxel = voxel_node;
                    hitData.voxelBufferIndex = index;
                    hitData.voxelSize = scale;
                    hitIndex = childGroupIndex;

                    index++;
                    minDistance = entryDist;
                } else {
                    stack[stackPos++] = Stack(childGroupIndex + countSetBitsBefore(childMask, i), newCenter, scale * 0.5f);
                }
            }
        }
    }

    if (hitData.anyHit){
        vec3 vSize = vec3(hitData.voxelSize);
        hitData.hitPosition = intersectRayAABB(ray, hitData.voxelPosition - vSize, hitData.voxelPosition + vSize).rgb;
        hitData.matData1 = getVoxel(hitIndex + matrialBufferOffset, bufferIndex);
        hitData.matData2 = getVoxel(hitIndex + matrialBufferOffset + 1, bufferIndex);
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
    if (!hitData.anyHit){
        hitData = trace(ray, 2
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 3
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 4
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 5
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 6
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 7
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 8
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    if (!hitData.anyHit){
        hitData = trace(ray, 9
        #ifdef DEBUG_VOXELS
        , showRaySearchCount, showRayTestCount, colorData
        #endif
        );
    }
    return hitData;
} 