#ifndef LIGHTDATA_H
#define LIGHTDATA_H
#include <glm/vec3.hpp>

namespace Metal {
    struct LightData final {
        alignas(4) unsigned int triangleIndex;
        alignas(4) unsigned int meshIndex;
        alignas(16) glm::vec3 color;
    };
}

#endif //LIGHTDATA_H
