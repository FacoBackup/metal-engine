#include "MeshBVH.h"
#include <chrono>
#include <limits>
#include <algorithm> // for std::swap
#include <glm/common.hpp>
#include "../../mesh/MeshData.h"
#include "RTTriangle.h"

namespace Metal {
    MeshBVH::MeshBVH(MeshData *mesh) {
        auto &indices = mesh->indices;
        auto &data = mesh->data;

        // Construct MeshBVH
        allNodes.clear();
        allTriangles.resize(indices.size() / 3);
        BVHBoundingBox bounds;

        // Build BVHTriangles and overall bounds.
        for (size_t i = 0; i < indices.size(); i += 3) {
            glm::vec3 a = data[indices[i + 0]].vertex;
            glm::vec3 b = data[indices[i + 1]].vertex;
            glm::vec3 c = data[indices[i + 2]].vertex;
            glm::vec3 centre = (a + b + c) / 3.0f;
            glm::vec3 maxVec = glm::max(glm::max(a, b), c);
            glm::vec3 minVec = glm::min(glm::min(a, b), c);
            allTriangles[i / 3] = BVHTriangle(centre, minVec, maxVec, static_cast<int>(i));
            bounds.GrowToInclude(minVec, maxVec);
        }

        // Create root node.
        allNodes.push_back(BottomLevelAccelerationStructure(bounds));
        Split(0, 0, static_cast<int>(allTriangles.size()));

        // Build final triangles from the BVHTriangles.
        allTris.resize(allTriangles.size());
        for (size_t i = 0; i < allTriangles.size(); i++) {
            BVHTriangle buildTri = allTriangles[i];
            auto &vertexData0 = data[indices[buildTri.Index + 0]];
            auto &vertexData1 = data[indices[buildTri.Index + 1]];
            auto &vertexData2 = data[indices[buildTri.Index + 2]];

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

    int MeshBVH::PushNode(const BottomLevelAccelerationStructure &node) {
        allNodes.push_back(node);
        return static_cast<int>(allNodes.size() - 1);
    }

    void MeshBVH::Split(int parentIndex, int triGlobalStart, int triNum, int depth) {
        const int MaxDepth = 32;
        BottomLevelAccelerationStructure &parent = allNodes[parentIndex];
        glm::vec3 size = parent.calculateBoundsSize();
        float parentCost = NodeCost(size, triNum);

        auto [splitAxis, splitPos, cost] = ChooseSplit(parent, triGlobalStart, triNum);

        if (cost < parentCost && depth < MaxDepth) {
            BVHBoundingBox boundsLeft, boundsRight;
            int numOnLeft = 0;

            // Partition the triangles based on the split position.
            for (int i = triGlobalStart; i < triGlobalStart + triNum; i++) {
                BVHTriangle &tri = allTriangles[i];
                if (tri.Centre[splitAxis] < splitPos) {
                    boundsLeft.GrowToInclude(tri.Min, tri.Max);
                    std::swap(allTriangles[triGlobalStart + numOnLeft], allTriangles[i]);
                    numOnLeft++;
                } else {
                    boundsRight.GrowToInclude(tri.Min, tri.Max);
                }
            }

            int numOnRight = triNum - numOnLeft;
            int triStartLeft = triGlobalStart;
            int triStartRight = triGlobalStart + numOnLeft;

            // Create child nodes.
            int childIndexLeft = PushNode(BottomLevelAccelerationStructure(boundsLeft, triStartLeft, 0));
            int childIndexRight = PushNode(BottomLevelAccelerationStructure(boundsRight, triStartRight, 0));

            // Mark parent as an internal node.
            parent.startIndex = childIndexLeft;
            parent.triangleCount = -1;

            // Recursively split the children.
            Split(childIndexLeft, triGlobalStart, numOnLeft, depth + 1);
            Split(childIndexRight, triGlobalStart + numOnLeft, numOnRight, depth + 1);
        } else {
            // This node becomes a leaf.
            parent.startIndex = triGlobalStart;
            parent.triangleCount = triNum;
        }
    }

    std::tuple<int, float, float> MeshBVH::ChooseSplit(const BottomLevelAccelerationStructure &node, int start,
                                                       int count) {
        if (count <= 1)
            return std::make_tuple(0, 0.0f, std::numeric_limits<float>::infinity());

        float bestSplitPos = 0.0f;
        int bestSplitAxis = 0;
        const int numSplitTests = 5;
        float bestCost = std::numeric_limits<float>::max();

        // Try several candidate split positions along each axis.
        for (int axis = 0; axis < 3; axis++) {
            for (int i = 0; i < numSplitTests; i++) {
                float splitT = (i + 1) / float(numSplitTests + 1);
                float splitPos = glm::mix(node.boundsMin[axis], node.boundsMax[axis], splitT);
                float cost = EvaluateSplit(axis, splitPos, start, count);
                if (cost < bestCost) {
                    bestCost = cost;
                    bestSplitPos = splitPos;
                    bestSplitAxis = axis;
                }
            }
        }

        return std::make_tuple(bestSplitAxis, bestSplitPos, bestCost);
    }

    float MeshBVH::EvaluateSplit(int splitAxis, float splitPos, int start, int count) {
        BVHBoundingBox boundsLeft, boundsRight;
        int numOnLeft = 0;
        int numOnRight = 0;

        for (int i = start; i < start + count; i++) {
            BVHTriangle &tri = allTriangles[i];
            if (tri.Centre[splitAxis] < splitPos) {
                boundsLeft.GrowToInclude(tri.Min, tri.Max);
                numOnLeft++;
            } else {
                boundsRight.GrowToInclude(tri.Min, tri.Max);
                numOnRight++;
            }
        }

        float costA = NodeCost(boundsLeft.Size(), numOnLeft);
        float costB = NodeCost(boundsRight.Size(), numOnRight);
        return costA + costB;
    }

    float MeshBVH::NodeCost(const glm::vec3 &size, int numTriangles) {
        float halfArea = size.x * size.y + size.x * size.z + size.y * size.z;
        return halfArea * numTriangles;
    }
} // Metal
