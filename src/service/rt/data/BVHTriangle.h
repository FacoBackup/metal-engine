#ifndef BVHTRIANGLE_H
#define BVHTRIANGLE_H

#include <glm/vec3.hpp>

namespace Metal {
    /**
     * A helper structure used during BVH construction.
     */
    class BVHTriangle {
    public:
        glm::vec3 Centre;
        glm::vec3 Min;
        glm::vec3 Max;
        int Index; // The starting index in the triangle index array.

        BVHTriangle() : Centre(0, 0, 0), Min(0, 0, 0), Max(0, 0, 0), Index(0) {
        }

        BVHTriangle(const glm::vec3 &centre, const glm::vec3 &min, const glm::vec3 &max, int index)
            : Centre(centre), Min(min), Max(max), Index(index) {
        }
    };
}
#endif // BVHTRIANGLE_H
