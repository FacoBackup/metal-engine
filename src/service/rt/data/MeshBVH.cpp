#include "MeshBVH.h"
#include <chrono>
#include <limits>
#include <algorithm> // for std::swap
#include <glm/common.hpp>
#include "../../mesh/MeshData.h"
#include "../../../dto/buffers/RTTriangle.h"

#define MAX_SPLIT_TESTS 5
#define MAX_DEPTH 32

namespace Metal {
    MeshBVH::MeshBVH(MeshData *mesh) {
        auto &indices = mesh->indices;
        auto &data = mesh->data;

        // Construct MeshBVH
        allNodes.clear();
        allTriangles.resize(indices.size() / 3);
        BVHBoundingBox bounds;
        for (size_t i = 0; i < indices.size(); i += 3) {
            glm::vec3 a = data[indices[i]].vertex;
            glm::vec3 b = data[indices[i + 1]].vertex;
            glm::vec3 c = data[indices[i + 2]].vertex;
            glm::vec3 centre = (a + b + c) / 3.0f;
            glm::vec3 max = glm::max(glm::max(a, b), c);
            glm::vec3 min = glm::min(glm::min(a, b), c);
            allTriangles.emplace_back(centre, min, max, i);
            bounds.growToInclude(min, max);
        }

        // Create the root node
        allNodes.emplace_back(bounds);
        // Recursively build the BVH
        split(0, 0, allTriangles.size());

        // Build final triangles from the BVHTriangles.
        allTris.resize(allTriangles.size());
        for (size_t i = 0; i < allTriangles.size(); i++) {
            BVHTriangle buildTri = allTriangles[i];
            auto &vertexData0 = data[indices[buildTri.index + 0]];
            auto &vertexData1 = data[indices[buildTri.index + 1]];
            auto &vertexData2 = data[indices[buildTri.index + 2]];

            allTris[i] = RTTriangle(
                vertexData0.vertex,
                vertexData1.vertex,
                vertexData2.vertex,
                vertexData0.normal,
                vertexData1.normal,
                vertexData2.normal,
                vertexData0.uv,
                vertexData1.uv,
                vertexData2.uv
            );
        }
    }

    void MeshBVH::split(int parentIndex, int triGlobalStart, int triNum, int depth) {
        BLAS parent = allNodes[parentIndex];
        glm::vec3 size = parent.boundsMax - parent.boundsMin;
        float parentCost = NodeCost(size, triNum);

        SplitResult split = ChooseSplit(parent, triGlobalStart, triNum);
        if (split.cost < parentCost && depth < MAX_DEPTH) {
            BVHBoundingBox boundsLeft, boundsRight;
            int numOnLeft = 0;

            // Partition triangles
            for (int i = triGlobalStart; i < triGlobalStart + triNum; ++i) {
                BVHTriangle &tri = allTriangles[i];
                if (tri.center[split.axis] < split.pos) {
                    boundsLeft.growToInclude(tri.min, tri.max);
                    std::swap(tri, allTriangles[triGlobalStart + numOnLeft]);
                    numOnLeft++;
                } else {
                    boundsRight.growToInclude(tri.min, tri.max);
                }
            }

            int numOnRight = triNum - numOnLeft;
            int childIndexLeft = static_cast<int>(allNodes.size());
            allNodes.emplace_back(boundsLeft, triGlobalStart, 0);
            int childIndexRight = static_cast<int>(allNodes.size());
            allNodes.emplace_back(boundsRight, triGlobalStart + numOnLeft, 0);

            // Update parent to point to children
            parent.startIndex = childIndexLeft;
            allNodes[parentIndex] = parent;

            // Recurse
            this->split(childIndexLeft, triGlobalStart, numOnLeft, depth + 1);
            this->split(childIndexRight, triGlobalStart + numOnLeft, numOnRight, depth + 1);
        } else {
            // Make leaf node
            parent.startIndex = triGlobalStart;
            parent.triangleCount = triNum;
            allNodes[parentIndex] = parent;
        }
    }

    SplitResult MeshBVH::ChooseSplit(const BLAS &node, int start, int count) {
        SplitResult best = {0, 0.0f, std::numeric_limits<float>::max()};
        if (count <= 1) return best;

        glm::vec3 nodeSize = node.boundsMax - node.boundsMin;

        for (int axis = 0; axis < 3; ++axis) {
            for (int i = 0; i < MAX_SPLIT_TESTS; ++i) {
                float t = (i + 1.0f) / (MAX_SPLIT_TESTS + 1.0f);
                float splitPos = node.boundsMin[axis] + t * nodeSize[axis];
                float cost = EvaluateSplit(axis, splitPos, start, count);

                if (cost < best.cost) {
                    best.axis = axis;
                    best.pos = splitPos;
                    best.cost = cost;
                }
            }
        }
        return best;
    }

    float MeshBVH::EvaluateSplit(int splitAxis, float splitPos, int start, int count) {
        BVHBoundingBox left, right;
        int numLeft = 0, numRight = 0;

        for (int i = start; i < start + count; ++i) {
            const BVHTriangle &tri = allTriangles[i];
            if (tri.center[splitAxis] < splitPos) {
                left.growToInclude(tri.min, tri.max);
                numLeft++;
            } else {
                right.growToInclude(tri.min, tri.max);
                numRight++;
            }
        }

        return NodeCost(left.Size(), numLeft) + NodeCost(right.Size(), numRight);
    }

    float MeshBVH::NodeCost(const glm::vec3 &size, int numTriangles) {
        float halfArea = size.x * size.y + size.x * size.z + size.y * size.z;
        return halfArea * numTriangles;
    }
} // Metal
