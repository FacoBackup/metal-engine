#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    struct VolumeData final {
        alignas(16) glm::vec4 color{};
        alignas(16) glm::vec3 position{};
        alignas(16) glm::vec3 dataA;
        alignas(16) glm::vec3 dataB;
        alignas(4) unsigned int itemType;
    };
}

#endif //VOLUMEDATA_H
