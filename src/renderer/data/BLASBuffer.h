#ifndef BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define BOTTOMLEVELACCELERATIONSTRUCTURE_H
#include <glm/vec3.hpp>
#include "BoundingBoxBVH.h"

namespace Metal {
    /**
     *                  Bottom level acceleration structure
     *
     * Stores a hierarchical reference to nodes that group triangles
     */
    struct BLASBuffer final {
        alignas(16) glm::vec3 boundsMin;
        alignas(16) glm::vec3 boundsMax;

        /**
        * When TriangleCount is negative (or not set), StartIndex is the index of the first child.
        * When positive, it is the index of the first triangle.
        */
        alignas(4) int startIndex;
        alignas(4) int triangleCount;

        /**
         * Root node only
         */
        explicit BLASBuffer(const BoundingBoxBVH &bounds)
            : boundsMin(bounds.Min), boundsMax(bounds.Max), startIndex(-1), triangleCount(-1) {
        }

        BLASBuffer(const BoundingBoxBVH &bounds, int startIndex, int triCount)
            : boundsMin(bounds.Min), boundsMax(bounds.Max), startIndex(startIndex), triangleCount(triCount) {
        }
    };
}

#endif //BOTTOMLEVELACCELERATIONSTRUCTURE_H
