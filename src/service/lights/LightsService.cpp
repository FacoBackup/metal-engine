#include "LightsService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void LightsService::registerLights() {
        // Register lights
        for (auto &entry: context.worldRepository.lights) {
            if (context.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = context.worldRepository.transforms.at(entry.first);
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

    void LightsService::registerSun() {
        if (context.engineRepository.atmosphereEnabled) {
            items.push_back(LightData(
                glm::vec4(sunColor, context.engineRepository.sunLightIntensity),
                sunPosition,
                glm::vec3(0),
                glm::vec3(context.engineRepository.sunRadius),
                LightVolumeTypes::SPHERE
            ));
        }
    }

    void LightsService::update() {
        items.clear();

        registerSun();
        registerLights();
        volumesOffset = items.size();
        registerVolumes();

        if (!items.empty()) {
            context.coreBuffers.lightsBuffer->update(items.data());
        }
        count = items.size();
    }


    void LightsService::computeSunInfo() {
        sunPosition = glm::vec3(0,
                                std::cos(context.engineRepository.elapsedTime),
                                std::sin(context.engineRepository.elapsedTime)) * context.engineRepository
                      .sunDistance;
        sunColor = LightsService::CalculateSunColor(
            sunPosition.y / context.engineRepository.sunDistance,
            context.engineRepository.nightColor, context.engineRepository.dawnColor,
            context.engineRepository.middayColor);
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
