#include "LightService.h"
#include <entt/entt.hpp>
#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/SphereLightComponent.h"
#include "../buffer/BufferInstance.h"
#include "../../enum/EngineResourceIDs.h"

namespace Metal {
    void LightService::registerLights() {
        auto view = CTX.worldRepository.registry.view<std::unique_ptr<LightComponent>, TransformComponent>();
        for (auto [entity, l_ptr, t]: view.each()) {
            const auto entityId = static_cast<EntityID>(entity);
            if (CTX.worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }
            auto &translation = t.translation;
            auto &l = *l_ptr;
            const auto lightType = l.getLightType();

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 rotatedNormal = t.rotation * normal;

            float radiusOrScale = 0;
            if (lightType == LightTypes::SPHERE) {
                radiusOrScale = static_cast<SphereLightComponent &>(l).radiusSize;
            }

            items.push_back(LightData(
                glm::vec4(l.color, l.intensity),
                translation,
                glm::normalize(rotatedNormal),
                lightType == LightTypes::SPHERE ? glm::vec3(radiusOrScale) : glm::vec3(t.scale),
                lightType
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
                LightTypes::SPHERE
            ));
        }
    }

    void LightService::onSync() {
        items.clear();

        registerSun();
        registerLights();

        if (!items.empty()) {
            CTX.engineContext.currentFrame->getResourceAs<BufferInstance>(RID_LIGHT_BUFFER)->update(items.data());
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
