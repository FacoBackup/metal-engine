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
        alignas(16) glm::vec3 cameraWorldPosition{};
        alignas(16) glm::vec3 sunColor{};
        alignas(16) glm::vec3 sunPosition{};
        alignas(4) float giStrength;
        alignas(4) unsigned int lightCount = 0;
        alignas(4) unsigned int volumeCount = 0;
        alignas(4) unsigned int volumeShadowSteps = 0;
        alignas(4) unsigned int rtTLASCount = 0;
        alignas(4) bool isAtmosphereEnabled;


        // GI
        alignas(4) bool enabledDenoiser;
        alignas(4) bool multipleImportanceSampling;
        alignas(4) unsigned int maxAccumulation;
        alignas(4) unsigned int giSamples;
        alignas(4) unsigned int giBounces;
        alignas(4) unsigned int giTileSubdivision;
        alignas(4) unsigned int surfaceCacheMinSamples;
        alignas(4) float giEmissiveFactor;

        alignas(4) unsigned int debugFlag;

        alignas(4) unsigned int surfaceCacheWidth;
        alignas(4) unsigned int surfaceCacheHeight;
        alignas(4) unsigned int globalFrameCount = 0;
    };
}

#endif
