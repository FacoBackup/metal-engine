#ifndef MESHBVH_H
#define MESHBVH_H
#include <vector>

#include "BottomLevelAccelerationStructure.h"
#include "BVHTriangle.h"

namespace Metal {
    struct VertexData;
}

namespace Metal {
    struct MeshData;
    struct RTTriangle;
    /**
     * The MeshBVH class constructs a bounding volume hierarchy from vertices, indices, and normals.
     */
    class MeshBVH {
    public:
        std::vector<BottomLevelAccelerationStructure> allNodes;
        std::vector<RTTriangle> allTris;

        explicit MeshBVH(MeshData *mesh);

    private:
        std::vector<BVHTriangle> allTriangles;

        void Split(int parentIndex,  int triGlobalStart, int triNum, int depth = 0);

        std::tuple<int, float, float> ChooseSplit(const BottomLevelAccelerationStructure &node, int start, int count);

        float EvaluateSplit(int splitAxis, float splitPos, int start, int count);

        static float NodeCost(const glm::vec3 &size, int numTriangles);

        int PushNode(const BottomLevelAccelerationStructure &node);
    };
} // Metal

#endif //MESHBVH_H
