#include "LightService.h"
#include "../../ApplicationContext.h"
#include "../resource/BufferInstance.h"
#include "../../editor/enum/EngineResourceIDs.h"
#include "../EngineContext.h"
#include "../repository/EngineRepository.h"

namespace Metal {
    void LightService::registerLights() {

    }

 // TODO - ADD EVENT SYSTEM THAT TRIGGERS THIS UPDATE
    void LightService::onSync() {
        items.clear();

        registerLights();

        if (!items.empty()) {
            engineContext->currentFrame->getResourceAs<BufferInstance>(RID_LIGHT_BUFFER)->update(items.data());
        }
    }

    void LightService::computeSunInfo() {
        sunPosition = glm::vec3(0,
                                std::cos(engineRepository->elapsedTime),
                                std::sin(engineRepository->elapsedTime)) * engineRepository->sunDistance;
        sunColor = LightService::CalculateSunColor(
            sunPosition.y / engineRepository->sunDistance,
            engineRepository->nightColor, engineRepository->dawnColor,
            engineRepository->middayColor);
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
