#ifndef BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define BOTTOMLEVELACCELERATIONSTRUCTURE_H
#include <glm/vec3.hpp>
#include "BVHBoundingBox.h"

namespace Metal {
    struct BottomLevelAccelerationStructure final {
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
        explicit BottomLevelAccelerationStructure(const BVHBoundingBox &bounds)
            : boundsMin(bounds.Min), boundsMax(bounds.Max), startIndex(1), triangleCount(-1) {
        }

        BottomLevelAccelerationStructure(const BVHBoundingBox &bounds, int startIndex, int triCount)
            : boundsMin(bounds.Min), boundsMax(bounds.Max), startIndex(startIndex), triangleCount(triCount) {
        }
    };
}

#endif //BOTTOMLEVELACCELERATIONSTRUCTURE_H
