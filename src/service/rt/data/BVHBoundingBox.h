#ifndef BVH_BOUNDINGBOX_H
#define BVH_BOUNDINGBOX_H
#include <glm/vec3.hpp>

namespace Metal {
    struct BVHBoundingBox final {
        glm::vec3 Min{};
        glm::vec3 Max{};
        bool hasPoint;

        BVHBoundingBox() : Min(0, 0, 0), Max(0, 0, 0), hasPoint(false) {
        }

        // Grow the bounding box to include the given min and max.
        void GrowToInclude(const glm::vec3 &min, const glm::vec3 &max) {
            if (hasPoint) {
                Min.x = (min.x < Min.x) ? min.x : Min.x;
                Min.y = (min.y < Min.y) ? min.y : Min.y;
                Min.z = (min.z < Min.z) ? min.z : Min.z;
                Max.x = (max.x > Max.x) ? max.x : Max.x;
                Max.y = (max.y > Max.y) ? max.y : Max.y;
                Max.z = (max.z > Max.z) ? max.z : Max.z;
            } else {
                hasPoint = true;
                Min = min;
                Max = max;
            }
        }

        glm::vec3 Centre() const {
            return (Min + Max) / 2.0f;
        }

        glm::vec3 Size() const {
            return Max - Min;
        }
    };
}

#endif // BVH_BOUNDINGBOX_H
