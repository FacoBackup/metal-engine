#ifndef BVHTRIANGLE_H
#define BVHTRIANGLE_H

#include <glm/vec3.hpp>

namespace Metal {
    /**
     * A helper structure used during BVH construction.
     */
    class BVHTriangle {
    public:
        glm::vec3 center;
        glm::vec3 min;
        glm::vec3 max;
        int index; // The starting index in the triangle index array.

        BVHTriangle() : center(0, 0, 0), min(0, 0, 0), max(0, 0, 0), index(0) {
        }

        BVHTriangle(const glm::vec3 &centre, const glm::vec3 &min, const glm::vec3 &max, int index)
            : center(centre), min(min), max(max), index(index) {
        }
    };
}
#endif // BVHTRIANGLE_H
