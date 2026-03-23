#include "LightService.h"
#include "../../ApplicationContext.h"
#include "../resource/BufferInstance.h"
#include "editor/enum/EngineResourceIDs.h"
#include "../EngineContext.h"
#include "../repository/EngineRepository.h"
#include "../repository/WorldRepository.h"
#include "../dto/PrimitiveComponent.h"
#include "../dto/MeshData.h"
#include "MeshService.h"
#include "common/LoggerUtil.h"
#include "engine/dto/LightComponent.h"
#include "editor/enum/engine-definitions.h"

namespace Metal {
    void LightService::onInitialize() {
        eventListener([this](const Event &) {
            needsUpdate = true;
        }, "BVHUpdated");
    }

    void LightService::registerLights() {
        for (const auto entity: worldRepository->registry.view<PrimitiveComponent, LightComponent>()) {
            auto &primitive = worldRepository->registry.get<PrimitiveComponent>(entity);
            if (!primitive.meshId.empty()) {
                if (MeshData *meshData = meshService->loadMeshData(primitive.meshId)) {
                    for (size_t i = 0; i < meshData->indices.size(); i += 3) {
                        LightData light{};
                        light.triangleIndex = static_cast<unsigned int>(i / 3);
                        light.meshIndex = primitive.renderIndex;
                        items.push_back(light);
                    }
                    delete meshData;
                }
            }
        }
    }

    void LightService::onSync() {
        if (!needsUpdate) return;
        needsUpdate = false;

        LOG_INFO("Updating lights");

        items.clear();

        registerLights();

        if (items.size() > MAX_LIGHTS) {
            LOG_WARN("Too many lights! Capping to " + std::to_string(MAX_LIGHTS));
            items.resize(MAX_LIGHTS);
        }

        lightCount = static_cast<int>(items.size());

        if (!items.empty()) {
            engineContext->currentFrame->getResourceAs<BufferInstance>(RID_LIGHT_BUFFER)->update(items.data(),
                items.size() * sizeof(LightData));
        }
    }

    void LightService::computeSunInfo() {
        sunPosition = glm::vec3(0,
                                std::cos(engineRepository->elapsedTime),
                                std::sin(engineRepository->elapsedTime)) * 10000.f;
    }

    glm::vec3 LightService::CalculateSunColor(const float elevation, const glm::vec3 &nightColor,
                                              const glm::vec3 &dawnColor,
                                              const glm::vec3 &middayColor) {
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

    glm::vec3 LightService::BlendColors(const glm::vec3 &c1, const glm::vec3 &c2, float t) {
        return {
            (c1.x * (1 - t) + c2.x * t),
            (c1.y * (1 - t) + c2.y * t),
            (c1.z * (1 - t) + c2.z * t)
        };
    }
} // Metal
