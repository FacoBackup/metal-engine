#include "./GlobalData.glsl"
#include "./CreateRay.glsl"

#define HALF_TILE_SIZE 16
#define MAX_VOXEL_SIZE 2000000
#define COUNT 32.
layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 finalColor;

#define DEFINE_OCTREE_BUFFER(B, M, N) \
layout(std430, set = 1, binding = B) readonly buffer M { \
    uint voxels[MAX_VOXEL_SIZE]; \
 } N;

DEFINE_OCTREE_BUFFER(0, V1, voxelBuffer1)
DEFINE_OCTREE_BUFFER(1, V2, voxelBuffer2)
DEFINE_OCTREE_BUFFER(2, V3, voxelBuffer3)
DEFINE_OCTREE_BUFFER(3, V4, voxelBuffer4)
DEFINE_OCTREE_BUFFER(4, V5, voxelBuffer5)
DEFINE_OCTREE_BUFFER(5, V6, voxelBuffer6)
DEFINE_OCTREE_BUFFER(6, V7, voxelBuffer7)
DEFINE_OCTREE_BUFFER(7, V8, voxelBuffer8)
DEFINE_OCTREE_BUFFER(8, V9, voxelBuffer9)

layout(set = 1, binding = 9) uniform TileInfo {
    vec4 tileCenterValid[9];
} tileInfo;

layout(push_constant) uniform Push {
    bool randomColors;
    bool showRaySearchCount;
    bool showRayTestCount;
} settings;

float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 71.9898, 78.233))) * 43758.5453);
}
vec3 randomColor(float seed) {
    float r = rand(vec3(seed));
    float g = rand(vec3(seed + r));
    return vec3(r, g, rand(vec3(seed + g)));
}

struct Ray { vec3 o, d, invDir; };
struct Stack {
    uint index;
    vec3 center;
    float scale;
};

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

// TODO - ADD DEFINE FOR DEBUG INFO
// TODO - EXTRACT TO SEPARATED GLSL FILE AND USE INCLUDE
// TODO - MAKE METHOD RETURN HIT VOXEL OR 0 IF NO VOXELS WERE HIT

// Based on https://www.shadertoy.com/view/MlBfRV
vec4 trace(
vec3 center,
Ray ray,
bool randomColors,
bool showRaySearchCount,
bool showRayTestCount
) {
    float scale = HALF_TILE_SIZE;
    vec3 minBox = center - scale;
    vec3 maxBox = center + scale;
    float minDistance = 1e10;// Large initial value
    if (!intersect(minBox, maxBox, ray)) return vec4(0);
    Stack stack[10];
    scale *= 0.5f;
    stack[0] = Stack(0u, center, scale);

    uint index = 0u;
    int rayTestCount = 0;
    int searchCount = 0;
    vec3 finalColor = vec3(0);
    int stackPos = 1;
    while (stackPos-- > 0) {
        if (showRaySearchCount){
            searchCount ++;
            finalColor.r = searchCount/COUNT;
        }
        center = stack[stackPos].center;
        index = stack[stackPos].index;
        scale = stack[stackPos].scale;

        uint voxel_node = voxelBuffer1.voxels[index];
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

            if (showRayTestCount){
                rayTestCount++;
                finalColor.g = rayTestCount/COUNT;
            }

            float entryDist;
            if (!intersectWithDistance(minBox, maxBox, ray, entryDist)) {
                continue;
            }
            if (entryDist < minDistance) {
                if (isLeafGroup) {
                    if (randomColors){
                        finalColor = randomColor(rand(newCenter.xyz));
                    } else {
                        finalColor = vec3(newCenter.y);
                    }
                    minDistance = entryDist;
                } else {
                    stack[stackPos++] = Stack(childGroupIndex + countSetBitsBefore(childMask, i), newCenter, scale * 0.5f);
                }
            }
        }
    }
    return vec4(finalColor, showRaySearchCount || showRayTestCount);
}

void main() {
    vec3 rayOrigin = globalData.cameraWorldPosition.xyz;
    vec3 rayDirection = createRay(texCoords, globalData.invProj, globalData.invView);
    vec4 outColor = trace(
        tileInfo.tileCenterValid[1].rgb * HALF_TILE_SIZE * 2,
        Ray(rayOrigin, rayDirection, 1./rayDirection),
        settings.randomColors,
        settings.showRaySearchCount,
        settings.showRayTestCount
    );

    if (length(outColor) > 0){
        finalColor = vec4(outColor.rgb, 1);
    } else {
        discard;
    }
}