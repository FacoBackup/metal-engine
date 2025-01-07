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
        alignas(4) unsigned int lightsQuantity = 0;
        alignas(4) bool enabledSun;
        alignas(4) bool giEnabled;

        // POST PROCESSING
        alignas(4) float distortionIntensity;
        alignas(4) float chromaticAberrationIntensity;
        alignas(4) bool distortionEnabled;
        alignas(4) bool chromaticAberrationEnabled;
        alignas(4) bool vignetteEnabled;
        alignas(4) float vignetteStrength;

        // GI
        alignas(4) unsigned int giBounces;
        alignas(4) unsigned int giSamplesPerPixel;
        alignas(4) unsigned int giTileSubdivision;

        alignas(4) unsigned int debugFlag;

        alignas(4) unsigned int giBufferWidth;
        alignas(4) unsigned int giBufferHeight;
        alignas(4) unsigned int giFrameCount;
    };
}

#endif
