#ifndef LIGHTDATA_H
#define LIGHTDATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Metal {
    struct LightData final {
        alignas(16) glm::vec3 color{};
        alignas(16) glm::vec3 position{};
        alignas(4) float innerRadius;
        alignas(4) float outerRadius;
        alignas(4) float radius;
    };
}

#endif //LIGHTDATA_H
