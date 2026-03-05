#include "LightService.h"
#include <entt/entt.hpp>
#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/SphereLightComponent.h"
#include "../buffer/BufferInstance.h"
#include "../../enum/EngineResourceIDs.h"

#include "../../repository/world/components/AtmosphereComponent.h"

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
        auto view = CTX.worldRepository.registry.view<AtmosphereComponent>();
        if (auto it = view.begin(); it != view.end()) {
            auto &atmo = CTX.worldRepository.registry.get<AtmosphereComponent>(*it);
            if (atmo.atmosphereEnabled) {
                items.push_back(LightData(
                        glm::vec4(atmo.sunColor, atmo.sunLightIntensity),
                        atmo.sunPosition,
                        glm::vec3(0),
                        glm::vec3(atmo.sunRadius),
                        LightTypes::SPHERE
                ));
            }
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
} // Metal
