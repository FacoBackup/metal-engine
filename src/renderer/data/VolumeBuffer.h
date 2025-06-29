#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    struct VolumeBuffer final {
        alignas(16) glm::vec4 color{};
        alignas(16) glm::vec3 position{};
        alignas(16) glm::vec3 size;
        alignas(4) float density;
        alignas(4) float scatteringAlbedo;
        alignas(4) float phaseFunctionAsymmetry;
    };
}

#endif //VOLUMEDATA_H
