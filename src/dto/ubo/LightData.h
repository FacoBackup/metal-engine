#ifndef LIGHTDATA_H
#define LIGHTDATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Metal {
    struct LightData final {
        alignas(16) glm::vec3 color{};
        alignas(16) glm::vec3 position{};
        alignas(16) glm::vec3 minNormal;
        alignas(16) glm::vec3 max;
        alignas(4) unsigned int lightType;
        alignas(4) float radiusSize;
    };
}

#endif //LIGHTDATA_H
