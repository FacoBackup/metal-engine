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
        alignas(4) float pathTracerMultiplier;
        alignas(4) unsigned int lightVolumeCount = 0;
        alignas(4) unsigned int volumesOffset = 0;
        alignas(4) unsigned int volumeShadowSteps = 0;
        alignas(4) bool isAtmosphereEnabled;


        // GI
        alignas(4) bool enabledDenoiser;
        alignas(4) bool multipleImportanceSampling;
        alignas(4) unsigned int pathTracerMaxSamples;
        alignas(4) unsigned int pathTracerSamples;
        alignas(4) unsigned int pathTracerBounces;
        alignas(4) unsigned int giTileSubdivision;
        alignas(4) float giEmissiveFactor;

        alignas(4) unsigned int debugFlag;

        alignas(4) unsigned int surfaceCacheWidth;
        alignas(4) unsigned int surfaceCacheHeight;
        alignas(4) unsigned int pathTracerAccumulationCount = 0;
        alignas(4) unsigned int globalFrameCount = 0;
        alignas(8) glm::vec2 outputRes{};
    };
}

#endif
