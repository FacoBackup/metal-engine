#ifndef ATMOSPHEREUBO_H
#define ATMOSPHEREUBO_H

#include <glm/vec3.hpp>

namespace Metal {
    struct AtmosphereUBO {
        alignas(16) glm::vec3 albedo;
        alignas(16) glm::vec3 volumeScale;
        alignas(4) unsigned int isAtmosphereEnabled;
        alignas(4) unsigned int isVolumeEnabled;
        alignas(4) unsigned int volumeShadowSteps;
        alignas(4) float density;
        alignas(4) float g;
        alignas(4) float scatteringAlbedo;
        alignas(4) int samples;
        alignas(16) glm::vec3 sunPosition{};
    };
}

#endif //ATMOSPHEREUBO_H
