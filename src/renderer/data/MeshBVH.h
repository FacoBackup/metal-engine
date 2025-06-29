#ifndef MESHBVH_H
#define MESHBVH_H
#include <vector>

#include "BLASBuffer.h"
#include "TriangleBVH.h"

namespace Metal {
    struct VertexData;
    struct MeshData;
    struct TriangleBuffer;

    struct SplitResult {
        int axis;
        float pos;
        float cost;
    };

    /**
     * The MeshBVH class constructs a bounding volume hierarchy from vertices, indices, and normals.
     */
    class MeshBVH {
    public:
        std::vector<BLASBuffer> allNodes;
        std::vector<TriangleBuffer> allTris;

        explicit MeshBVH(MeshData *mesh);

    private:
        std::vector<TriangleBVH> allTriangles;

        void split(int parentIndex,
                   int triGlobalStart,
                   int triNum,
                   int depth = 0);

        SplitResult ChooseSplit(const BLASBuffer &node, int start, int count);

        float EvaluateSplit(int splitAxis, float splitPos, int start, int count);

        static float NodeCost(const glm::vec3 &size, int numTriangles);
    };
} // Metal

#endif //MESHBVH_H
