#include "LightsService.h"
#include "../../MetalContextProvider.h"
#include "../data/BufferInstance.h"

namespace Metal {
    void LightsService::registerLights() {
        // Register lights
        for (auto &entry: SINGLETONS.worldRepository.lights) {
            if (SINGLETONS.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = SINGLETONS.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 rotatedNormal = t.rotation * normal;

            items.push_back(LightBuffer(
                glm::vec4(l.color, l.intensity),
                translation,
                glm::normalize(rotatedNormal),
                l.lightType == LightVolumeTypes::LightVolumeType::SPHERE ? glm::vec3(l.radiusSize) : glm::vec3(t.scale),
                l.lightType
            ));
        }
    }

    void LightsService::registerSun() {
        if (SINGLETONS.engineRepository.atmosphereEnabled) {
            items.push_back(LightBuffer(
                glm::vec4(sunColor, SINGLETONS.engineRepository.sunLightIntensity),
                sunPosition,
                glm::vec3(0),
                glm::vec3(SINGLETONS.engineRepository.sunRadius),
                LightVolumeTypes::SPHERE
            ));
        }
    }

    void LightsService::update() {
        items.clear();

        registerSun();
        registerLights();

        if (!items.empty()) {
            SINGLETONS.coreBuffers.lightsBuffer->update(items.data());
        }
        count = items.size();
    }


    void LightsService::computeSunInfo() {
        if (!SINGLETONS.engineRepository.atmosphereEnabled) {
            return;
        }
        sunPosition = glm::vec3(0,
                                std::cos(SINGLETONS.engineRepository.elapsedTime),
                                std::sin(SINGLETONS.engineRepository.elapsedTime)) * SINGLETONS.engineRepository
                      .sunDistance;
        sunColor = LightsService::CalculateSunColor(
            sunPosition.y / SINGLETONS.engineRepository.sunDistance,
            SINGLETONS.engineRepository.nightColor, SINGLETONS.engineRepository.dawnColor,
            SINGLETONS.engineRepository.middayColor);
    }

    glm::vec3 LightsService::CalculateSunColor(const float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                               glm::vec3 &middayColor) {
        if (elevation <= -0.1f) {
            return nightColor;
        }
        if (elevation <= 0.0f) {
            float t = (elevation + 0.1f) / 0.1f;
            return BlendColors(nightColor, dawnColor, t);
        }
        if (elevation <= 0.5f) {
            float t = elevation / 0.5f;
            return BlendColors(dawnColor, middayColor, t);
        }
        return middayColor;
    }

    glm::vec3 LightsService::BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t) {
        return {
            (c1.x * (1 - t) + c2.x * t),
            (c1.y * (1 - t) + c2.y * t),
            (c1.z * (1 - t) + c2.z * t)
        };
    }
} // Metal
