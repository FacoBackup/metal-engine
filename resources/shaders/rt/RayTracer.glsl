#include "../rt/RTStructures.glsl"
#include "../GlobalDataBuffer.glsl"
// -------------------------
// Constants & Helper Types
// -------------------------
const float INF = 1e20;

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
    hit.didHit = false;// default

    vec3 edge1 = tri.v2 - tri.v1;
    vec3 edge2 = tri.v3 - tri.v1;
    vec3 normalVector = cross(edge1, edge2);
    vec3 ao = ray.origin - tri.v1;
    vec3 dao = cross(ao, ray.dir);

    float determinant = -dot(ray.dir, normalVector);
    if (determinant < 1e-8)
    return hit;// no hit (backface or parallel)

    float invDet = 1.0 / determinant;
    float t = dot(ao, normalVector) * invDet;
    float u = dot(edge2, dao) * invDet;
    float v = -dot(edge1, dao) * invDet;
    float w = 1.0 - u - v;

    if (t < 0.0 || u < 0.0 || v < 0.0 || w < 0.0)
    return hit;

    hit.closestT   = t;
    hit.hitPosition = ray.origin + ray.dir * t;
    hit.hitNormal  = normalize(tri.n1 * w + tri.n2 * u + tri.n3 * v);
    hit.hitUV      = tri.uv1 * w + tri.uv2 * u + tri.uv3 * v;
    hit.didHit     = true;
    return hit;
}

// -------------------------
// Ray - Bounding Box Intersection
// -------------------------
// Based on Tavianator’s algorithm.
float RayBoundingBoxDst(Ray ray, vec3 boxMin, vec3 boxMax) {
    vec3 tMin = (boxMin - ray.origin) * ray.invDir;
    vec3 tMax = (boxMax - ray.origin) * ray.invDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar  = min(min(t2.x, t2.y), t2.z);

    bool hit = (tFar >= tNear && tFar > 0.0);
    return hit ? (tNear > 0.0 ? tNear : 0.0) : INF;
}

// -------------------------
// BVH Traversal within a Bottom-Level AS
// -------------------------
// This function traverses the BLAS starting at the given node offset. For leaf nodes
// (triangleCount > 0), it tests each triangle from trianglesBuffer. For inner nodes,
// it computes the bounding-box intersection distances for both children and pushes the
// closer child last (so it’s processed first).
HitData RayTriangleBVH(Ray ray, float rayLength, int nodeOffset,
inout uint triangleTestCount, inout uint boxTestCount) {
    HitData result;
    result.closestT = rayLength;
    result.didHit = false;
    result.hitId = 0u;

    // Use a fixed-size stack (adjust the size if necessary)
    int stack[32];
    int stackIndex = 0;
    stack[stackIndex++] = nodeOffset;// Push the root node.

    while (stackIndex > 0) {
        int nodeIndex = stack[--stackIndex];
        BottomLevelAS node = blasBuffer.items[nodeIndex];

        if (node.triangleCount > 0) {
            // Leaf node: iterate over the triangles.
            for (int i = 0; i < node.triangleCount; i++) {
                int triIndex = node.startIndex + i;
                RTTriangle tri = trianglesBuffer.items[triIndex];
                triangleTestCount++;
                HitData triHit = RayTriangle(ray, tri);
                if (triHit.didHit && triHit.closestT < result.closestT) {
                    result = triHit;
                    // The hitId will be set later in the TLAS loop.
                }
            }
        } else {
            // Inner node: retrieve children.
            int childIndexA = node.startIndex + nodeOffset;
            int childIndexB = node.startIndex + nodeOffset + 1;
            BottomLevelAS childA = blasBuffer.items[childIndexA];
            BottomLevelAS childB = blasBuffer.items[childIndexB];

            float dstA = RayBoundingBoxDst(ray, childA.boundsMin, childA.boundsMax);
            float dstB = RayBoundingBoxDst(ray, childB.boundsMin, childB.boundsMax);
            boxTestCount += 2u;

            // Push the farther child first so that the closer child is processed next.
            bool isNearestA = dstA <= dstB;
            float dstNear = isNearestA ? dstA : dstB;
            float dstFar  = isNearestA ? dstB : dstA;
            int childNear = isNearestA ? childIndexA : childIndexB;
            int childFar  = isNearestA ? childIndexB : childIndexA;

            if (dstFar < result.closestT)
            stack[stackIndex++] = childFar;
            if (dstNear < result.closestT)
            stack[stackIndex++] = childNear;
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
        localRay.origin = (instance.transform * vec4(rayOrigin, 1.0)).xyz;
        localRay.dir    = (instance.transform * vec4(rayDir, 0.0)).xyz;
        localRay.invDir = 1.0 / localRay.dir;

        // Reset local counters for this instance.
        uint localTriangleCount = 0u;
        uint localBoxCount = 0u;
        int nodeOffset = int(instance.bottomLevelASOffset);

        // Traverse the BLAS for this instance.
        HitData localHit = RayTriangleBVH(localRay, result.closestT, nodeOffset,
        localTriangleCount, localBoxCount);

        // Accumulate test counts.
        result.triangleTestCount += localTriangleCount;
        result.boxTestCount += localBoxCount;

        if (localHit.didHit && localHit.closestT < result.closestT) {
            // Transform the hit position back to world space.
            result.closestT = localHit.closestT;
            result.hitPosition = (instance.transform * vec4(localHit.hitPosition, 1.0)).xyz;
            // Transform the normal properly (using the inverse transpose of the instance transform).
            result.hitNormal = normalize((transpose(inverse(instance.transform)) * vec4(localHit.hitNormal, 0.0)).xyz);
            result.hitUV = localHit.hitUV;
            result.didHit = true;
            // Use the TLAS instance’s id as the hit identifier.
            result.hitId = instance.id;
        }
    }

    return result;
}
