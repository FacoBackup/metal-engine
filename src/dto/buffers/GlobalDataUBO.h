#ifndef GLOBALDATAUBO_H
#define GLOBALDATAUBO_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct GlobalDataUBO {
        alignas(16) glm::mat4x4 viewMatrix{};
        alignas(16) glm::mat4x4 projectionMatrix{};
        alignas(16) glm::mat4x4 projView{};
        alignas(16) glm::mat4x4 invView{};
        alignas(16) glm::mat4x4 invProj{};
        alignas(16) glm::mat4x4 previousProjView{};
        alignas(16) glm::vec3 cameraWorldPosition{};
        alignas(16) glm::vec3 sunColor{};
        alignas(16) glm::vec3 sunPosition{};
        alignas(4) unsigned int volumeCount{};
        alignas(4) unsigned int lightsCount{};
        alignas(4) unsigned int debugFlag;
        alignas(4) unsigned int pathTracerAccumulationCount = 0;
        alignas(4) unsigned int globalFrameCount = 0;
        alignas(4) unsigned int pathTracerMaxSamples{};
        alignas(4) unsigned int denoiserEnabled;

    };
}

#endif
