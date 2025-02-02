#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Metal {
    struct VolumeData final {
        alignas(16) glm::vec3 albedo{};
        alignas(16) glm::vec3 position{};
        alignas(16) glm::vec3 dimensions;
        alignas(4)  float density;
    };
}
#endif //VOLUMEDATA_H
