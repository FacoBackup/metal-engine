#include "LightService.h"
#include <entt/entt.hpp>
#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/SphereLightComponent.h"
#include "../../repository/world/components/PlaneLightComponent.h"
#include "../buffer/BufferInstance.h"
#include "../../enum/EngineResourceIDs.h"

#include "../../enum/LightType.h"
#include "../../repository/world/components/AtmosphereComponent.h"

namespace Metal {
    void LightService::registerLights() {
        auto sphereView = CTX.worldRepository.registry.view<SphereLightComponent, TransformComponent>();
        for (auto [entityId, l, t]: sphereView.each()) {

            if (CTX.worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }
            auto &translation = t.translation;

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 rotatedNormal = t.rotation * normal;

            items.push_back(LightData(
                glm::vec4(l.color, l.intensity),
                translation,
                glm::normalize(rotatedNormal),
                glm::vec3(l.radiusSize),
                LightTypes::LightType::SPHERE
            ));
        }

        auto planeView = CTX.worldRepository.registry.view<PlaneLightComponent, TransformComponent>();
        for (auto [entityId, l, t]: planeView.each()) {

            if (CTX.worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }
            auto &translation = t.translation;

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 rotatedNormal = t.rotation * normal;

            items.push_back(LightData(
                glm::vec4(l.color, l.intensity),
                translation,
                glm::normalize(rotatedNormal),
                glm::vec3(t.scale),
                LightTypes::LightType::PLANE
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
                        LightTypes::LightType::SPHERE
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
