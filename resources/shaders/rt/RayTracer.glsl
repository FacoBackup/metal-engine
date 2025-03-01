#ifndef RAYTRACER
#define RAYTRACER

#include "../rt/RTStructures.glsl"
#include "../GlobalDataBuffer.glsl"

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

// -------------------------
// BVH Traversal within a Bottom-Level Acceleration structure
// -------------------------
HitData RayTriangleBVH(
Ray ray,
float rayLength,
uint nodeOffset,
uint triangleOffset
) {
    HitData result;
    result.closestT = rayLength;
    result.didHit = false;
    result.hitId = 0u;

    // Use a fixed-size stack (adjust the size if necessary)
    uint stack[32];
    int stackIndex = 0;
    stack[stackIndex++] = nodeOffset;// Push the root node.
    while (stackIndex > 0) {
        uint nodeIndex = stack[--stackIndex];
        BottomLevelAS node = blasBuffer.items[nodeIndex];
        if (node.triangleCount > 0) {
            // Leaf node: iterate over the triangles.
            for (uint i = 0; i < node.triangleCount; i++) {
                uint triIndex = node.startIndex + i + triangleOffset;
                RTTriangle tri = trianglesBuffer.items[triIndex];
                HitData triHit = RayTriangle(ray, tri);
                triHit.triangleId = triIndex;
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

    // Loop over each TLAS instance.
    for (uint i = 0u; i < globalData.rtTLASCount; i++) {
        TopLevelAS instance = tlasBuffer.items[i];

        Ray localRay;
        localRay.origin = (instance.invTransform * vec4(rayOrigin, 1.0)).xyz;
        localRay.dir    = (instance.invTransform * vec4(rayDir, 0.0)).xyz;
        localRay.invDir = 1.0 / localRay.dir;

        // Traverse the BLAS for this instance.
        HitData localHit = RayTriangleBVH(
        localRay,
        result.closestT,
        instance.nodeOffset,
        instance.triangleOffset);

        if (localHit.didHit && localHit.closestT < result.closestT) {
            result.closestT = localHit.closestT;
            result.hitPosition = (inverse(instance.invTransform) * vec4(localHit.hitPosition, 1.0)).xyz;
            result.hitNormal = normalize((transpose(instance.invTransform) * vec4(localHit.hitNormal, 0.0)).xyz);
            result.hitUV = localHit.hitUV;
            result.didHit = true;
            result.hitId = instance.id;
            result.materialId = instance.materialId;
            result.incomingRayDir = rayDir;
        }
    }

    return result;
}
#endif