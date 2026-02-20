#include "LightService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void LightService::registerLights() {
        for (auto &entry: CTX.worldRepository.lights) {
            if (CTX.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = CTX.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 rotatedNormal = t.rotation * normal;

            items.push_back(LightData(
                glm::vec4(l.color, l.intensity),
                translation,
                glm::normalize(rotatedNormal),
                l.lightType == LightVolumeTypes::LightVolumeType::SPHERE ? glm::vec3(l.radiusSize) : glm::vec3(t.scale),
                l.lightType
            ));
        }
    }

    void LightService::registerSun() {
        if (CTX.engineRepository.atmosphereEnabled) {
            items.push_back(LightData(
                glm::vec4(sunColor, CTX.engineRepository.sunLightIntensity),
                sunPosition,
                glm::vec3(0),
                glm::vec3(CTX.engineRepository.sunRadius),
                LightVolumeTypes::SPHERE
            ));
        }
    }

    void LightService::onSync() {
        items.clear();

        registerSun();
        registerLights();

        if (!items.empty()) {
            CTX.coreBuffers.lightBuffer->update(items.data());
        }
    }


    void LightService::computeSunInfo() {
        sunPosition = glm::vec3(0,
                                std::cos(CTX.engineRepository.elapsedTime),
                                std::sin(CTX.engineRepository.elapsedTime)) * CTX.engineRepository
                      .sunDistance;
        sunColor = LightService::CalculateSunColor(
            sunPosition.y / CTX.engineRepository.sunDistance,
            CTX.engineRepository.nightColor, CTX.engineRepository.dawnColor,
            CTX.engineRepository.middayColor);
    }

    glm::vec3 LightService::CalculateSunColor(const float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
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

    glm::vec3 LightService::BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t) {
        return {
            (c1.x * (1 - t) + c2.x * t),
            (c1.y * (1 - t) + c2.y * t),
            (c1.z * (1 - t) + c2.z * t)
        };
    }
} // Metal
