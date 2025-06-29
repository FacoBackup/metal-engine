#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Metal {
    struct TriangleBuffer final {
        alignas(16) glm::vec3 v1;
        alignas(16) glm::vec3 v2;
        alignas(16) glm::vec3 v3;

        alignas(16) glm::vec3 n1;
        alignas(16) glm::vec3 n2;
        alignas(16) glm::vec3 n3;

        alignas(8) glm::vec2 uv1;
        alignas(8) glm::vec2 uv2;
        alignas(8) glm::vec2 uv3;
    };
}
#endif //TRIANGLE_H
