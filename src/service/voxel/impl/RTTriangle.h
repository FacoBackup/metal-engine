#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Metal {
    struct RTTriangle {
        glm::vec3 v0{};
        glm::vec3 v1{};
        glm::vec3 v2{};

        glm::vec3 n0{};
        glm::vec3 n1{};
        glm::vec3 n2{};

        glm::vec2 uv0{};
        glm::vec2 uv1{};
        glm::vec2 uv2{};
    };
}

#endif //TRIANGLE_H
