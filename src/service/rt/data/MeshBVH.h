#ifndef MESHBVH_H
#define MESHBVH_H
#include <vector>

#include "../../../dto/buffers/BLAS.h"
#include "BVHTriangle.h"

namespace Metal {
    struct VertexData;
    struct MeshData;
    struct RTTriangle;

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
        std::vector<BLAS> allNodes;
        std::vector<RTTriangle> allTris;

        explicit MeshBVH(MeshData *mesh);

    private:
        std::vector<BVHTriangle> allTriangles;

        void split(int parentIndex,
                   int triGlobalStart,
                   int triNum,
                   int depth = 0);

        SplitResult ChooseSplit(const BLAS &node, int start, int count);

        float EvaluateSplit(int splitAxis, float splitPos, int start, int count);

        static float NodeCost(const glm::vec3 &size, int numTriangles);
    };
} // Metal

#endif //MESHBVH_H
