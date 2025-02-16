#include "../rt/RTStructures.glsl"
#include "../GlobalDataBuffer.glsl"

// ------------------------------
// Helper: Ray-AABB intersection
bool intersectAABB(vec3 rayOrigin, vec3 invRayDir, vec3 boxMin, vec3 boxMax, out float tNear, out float tFar) {
    vec3 t0s = (boxMin - rayOrigin) * invRayDir;
    vec3 t1s = (boxMax - rayOrigin) * invRayDir;
    vec3 tmin = min(t0s, t1s);
    vec3 tmax = max(t0s, t1s);
    tNear = max(max(tmin.x, tmin.y), tmin.z);
    tFar  = min(min(tmax.x, tmax.y), tmax.z);
    return tFar >= max(tNear, 0.0);
}

// ------------------------------
// Helper: Möller–Trumbore triangle intersection
bool intersectTriangle(vec3 orig, vec3 dir, RTTriangle tri, out float t, out float u, out float v) {
    const float EPSILON = 1e-6;
    vec3 edge1 = tri.v2 - tri.v1;
    vec3 edge2 = tri.v3 - tri.v1;
    vec3 h = cross(dir, edge2);
    float a = dot(edge1, h);
    if (abs(a) < EPSILON)
    return false;
    float f = 1.0 / a;
    vec3 s = orig - tri.v1;
    u = f * dot(s, h);
    if (u < 0.0 || u > 1.0)
    return false;
    vec3 q = cross(s, edge1);
    v = f * dot(dir, q);
    if (v < 0.0 || (u + v) > 1.0)
    return false;
    t = f * dot(edge2, q);
    return t > EPSILON;
}

HitData trace(vec3 rayOrigin, vec3 rayDir){
    // ------------------------------
    // Initialize hit info
    HitData data;
    data.closestT = 1e20;
    data.didHit = false;
    // --- Loop over TLAS instances ---
    for (int i = 0; i < globalData.rtTLASCount; ++i) {
        TopLevelAS instance = tlasBuffer.items[i];
        // Transform the ray into the instance's (BLAS’s) local space.
        mat4 invTransform = inverse(transformationsBuffer.items[instance.transform]);
        vec3 localOrigin = (invTransform * vec4(rayOrigin, 1.0)).xyz;
        vec3 localDir    = normalize((invTransform * vec4(rayDir, 0.0)).xyz);
        vec3 invDir   = 1.0 / localDir;

        // Retrieve the corresponding BLAS
        BottomLevelAS blas = blasBuffer.items[instance.bottomLevelASOffset];
        vec3 boxMin = blas.minCount.xyz;
        vec3 boxMax = blas.maxOffset.xyz;

        // Test against the BLAS bounding box.
        float tNear, tFar;
        if (!intersectAABB(localOrigin, invDir, boxMin, boxMax, tNear, tFar))
        continue;// skip this instance

        // Determine the triangle range within the triangle buffer.
        int triCount = int(blas.minCount.w);
        int triOffset = int(blas.maxOffset.w);

        // --- Loop over triangles in this BLAS ---
        for (int j = 0; j < triCount; ++j) {
            RTTriangle tri = trianglesBuffer.items[triOffset + j];
            float t, u, v;
            if (intersectTriangle(localOrigin, localDir, tri, t, u, v)) {
                // Check if this is the closest hit so far.
                if (t < data.closestT) {
                    data.closestT = t;
                    data.didHit = true;

                    data.hitNormal = normalize(mix(mix(tri.n1, tri.n2, u), tri.n3, v));
                    data.hitUV = normalize(mix(mix(tri.uv1, tri.uv2, u), tri.uv3, v));
                    data.hitPosition = normalize(mix(mix(tri.v1, tri.v2, u), tri.v3, v));
                }
            }
        }
    }

    return data;
}