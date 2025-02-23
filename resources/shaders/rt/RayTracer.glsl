#include "../rt/RTStructures.glsl"
#include "../GlobalDataBuffer.glsl"
// -------------------------
// Constants & Helper Types
// -------------------------
#define INF 1e20
#define EPSILON 1e-6

struct Ray {
    vec3 origin;
    vec3 dir;
    vec3 invDir;
};

// -------------------------
// Ray - Triangle Intersection
// -------------------------
// This function uses the Möller–Trumbore algorithm. It computes barycentrics (u,v,w)
// and uses them to blend the vertex normals and UVs.
HitData RayTriangle(Ray ray, RTTriangle tri) {
    HitData hit;
    hit.didHit = false;

    float t, u, v;
    vec3 edge1 = tri.v2 - tri.v1;
    vec3 edge2 = tri.v3 - tri.v1;
    vec3 h = cross(ray.dir, edge2);
    float a = dot(edge1, h);
    if (abs(a) < EPSILON){
        return hit;
    }
    float f = 1.0 / a;
    vec3 s = ray.origin - tri.v1;
    u = f * dot(s, h);
    if (u < 0.0 || u > 1.0){
        return hit;
    }
    vec3 q = cross(s, edge1);
    v = f * dot(ray.dir, q);
    if (v < 0.0 || (u + v) > 1.0){
        return hit;
    }
    t = f * dot(edge2, q);
    if (t <= EPSILON){
        return hit;
    }

    hit.closestT    = t;

    float w = 1.0 - u - v;

    hit.hitNormal = normalize(tri.n1 * w + tri.n2 * u + tri.n3 * v);
    hit.hitUV = tri.uv1 * w + tri.uv2 * u + tri.uv3 * v;
    hit.hitPosition = tri.v1 * w + tri.v2 * u + tri.v3 * v;

    hit.didHit      = true;
    return hit;
}

// -------------------------
// Ray - Bounding Box Intersection
// -------------------------
// Based on Tavianator’s algorithm.
float RayBoundingBoxDst(Ray ray, vec3 boxMin, vec3 boxMax) {
    vec3 t0s = (boxMin - ray.origin) * ray.invDir;
    vec3 t1s = (boxMax - ray.origin) * ray.invDir;
    vec3 tmin = min(t0s, t1s);
    vec3 tmax = max(t0s, t1s);
    float tNear = max(max(tmin.x, tmin.y), tmin.z);
    float tFar  = min(min(tmax.x, tmax.y), tmax.z);
    bool hit = tFar >= max(tNear, 0.0);
    return hit ? (tNear > 0.0 ? tNear : 0.0) : INF;
}

// Returns true if the ray intersects the box.
// Outputs tHit (distance along the ray) and hitPos (intersection point).
bool intersectBox(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax, inout float tHit, inout vec3 hitPos) {
    // Compute the inverse of the ray direction.
    vec3 invDir = 1.0 / rayDir;

    // Compute the intersection distances for the two slabs for each axis.
    vec3 t0 = (boxMin - rayOrigin) * invDir;
    vec3 t1 = (boxMax - rayOrigin) * invDir;

    // Determine the nearest and farthest intersection distances.
    vec3 tMin = min(t0, t1);
    vec3 tMax = max(t0, t1);

    // The nearest possible intersection is the maximum of the tMin values.
    float tNear = max(max(tMin.x, tMin.y), tMin.z);
    // The farthest intersection is the minimum of the tMax values.
    float tFar = min(min(tMax.x, tMax.y), tMax.z);

    // If tNear is greater than tFar, the ray misses the box.
    // Also, if tFar is negative, the box is behind the ray.
    if (tNear > tFar || tFar < 0.0)
    return false;

    // Use tNear as the intersection distance and compute the hit position.
    tHit = tNear;
    hitPos = rayOrigin + rayDir * tHit;

    return true;
}


// -------------------------
// BVH Traversal within a Bottom-Level Acceleration structure
// -------------------------
HitData RayTriangleBVH(
Ray ray,
float rayLength,
uint nodeOffset,
uint triangleOffset,
inout uint triangleTestCount,
inout uint boxTestCount
) {
    HitData result;
    result.closestT = rayLength;
    result.didHit = false;
    result.hitId = 0u;

    // Use a fixed-size stack (adjust the size if necessary)
    uint stack[32];
    int stackIndex = 0;
    stack[stackIndex++] = nodeOffset;// Push the root node.
    int totalLoops = 0;
    while (stackIndex > 0) {
        if (totalLoops > 100000){
            break;
        }
        totalLoops++;
        uint nodeIndex = stack[--stackIndex];
        BottomLevelAS node = blasBuffer.items[nodeIndex];
        if (boxTestCount > globalData.searchCountDivisor && globalData.searchCountDivisor > 0){
            float tHit = 0;
            vec3 p = vec3(0);
            if (intersectBox(ray.origin, ray.dir, node.boundsMin, node.boundsMax, tHit, p)){
                result.hitPosition = (node.boundsMin + node.boundsMax) / 2;
                result.didHit = true;
                result.closestT = tHit;
                result.hitId = stackIndex;
                return result;
            }
        }
        if (node.triangleCount > 0) {
            // Leaf node: iterate over the triangles.
            for (uint i = 0; i < node.triangleCount; i++) {
                uint triIndex = node.startIndex + i + triangleOffset;
                RTTriangle tri = trianglesBuffer.items[triIndex];
                triangleTestCount++;
                HitData triHit = RayTriangle(ray, tri);
                if (triHit.didHit && triHit.closestT < result.closestT) {
                    result = triHit;
                }
            }
        } else {
            // Inner node: process children
            uint childIndexA = nodeOffset + node.startIndex;
            uint childIndexB = childIndexA + 1;


            BottomLevelAS childA = blasBuffer.items[childIndexA];
            BottomLevelAS childB = blasBuffer.items[childIndexB];

            // Compute intersection distances
            float dstA = RayBoundingBoxDst(ray, childA.boundsMin, childA.boundsMax);
            float dstB = RayBoundingBoxDst(ray, childB.boundsMin, childB.boundsMax);
            boxTestCount += 2u;

            // Push farther child first
            bool isNearestA = dstA <= dstB;
            float dstNear = isNearestA ? dstA : dstB;
            float dstFar = isNearestA ? dstB : dstA;
            uint childNear = isNearestA ? childIndexA : childIndexB;
            uint childFar = isNearestA ? childIndexB : childIndexA;

            if (dstFar < result.closestT) {
                stack[stackIndex++] = childFar;
            }
            if (dstNear < result.closestT) {
                stack[stackIndex++] = childNear;
            }

        }
    }

    return result;
}

// -------------------------
// TLAS Traversal & Collision Calculation
// -------------------------
// For each TLAS instance, we transform the world-space ray into the instance’s local
// coordinate system and traverse its BLAS. If a hit is found that is closer than any
// previous hit, we transform the hit information back to world space and record it.
HitData trace(vec3 rayOrigin, vec3 rayDir) {
    HitData result;
    result.closestT = INF;
    result.didHit = false;
    result.triangleTestCount = 0u;
    result.boxTestCount = 0u;

    // Loop over each TLAS instance.
    for (uint i = 0u; i < globalData.rtTLASCount; i++) {
        TopLevelAS instance = tlasBuffer.items[i];

        Ray localRay;
        localRay.origin = (instance.invTransform * vec4(rayOrigin, 1.0)).xyz;
        localRay.dir    = (instance.invTransform * vec4(rayDir, 0.0)).xyz;
        localRay.invDir = 1.0 / localRay.dir;

        // Reset local counters for this instance.
        uint localTriangleCount = 0u;
        uint localBoxCount = 0u;

        // Traverse the BLAS for this instance.
        HitData localHit = RayTriangleBVH(
        localRay,
        result.closestT,
        instance.nodeOffset,
        instance.triangleOffset,
        localTriangleCount,
        localBoxCount);

        result.triangleTestCount += localTriangleCount;
        result.boxTestCount += localBoxCount;

        if (localHit.didHit && localHit.closestT < result.closestT) {
            result.closestT = localHit.closestT;
            result.hitPosition = (inverse(instance.invTransform) * vec4(localHit.hitPosition, 1.0)).xyz;
            result.hitNormal = normalize((transpose(instance.invTransform) * vec4(localHit.hitNormal, 0.0)).xyz);
            result.hitUV = localHit.hitUV;
            result.didHit = true;
            result.hitId = instance.id;
        }
    }

    return result;
}
