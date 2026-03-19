#include "TransformService.h"
#include <entt/entt.hpp>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include "../../common/LoggerUtil.h"

#include "../../ApplicationContext.h"
#include "../dto/TransformComponent.h"
#include "../repository/WorldRepository.h"
#include "../dto/TransformComponent.h"
#include "../dto/PrimitiveComponent.h"
#include "RayTracingService.h"

namespace Metal {
    void TransformService::onSync() {
        for (auto entity : worldRepository->registry.view<TransformComponent>()) {
            TransformComponent &st = worldRepository->registry.get<TransformComponent>(entity);
            if (st.isNotFrozen()) {
                transform(&st, nullptr);
                st.freezeVersion();
            }
        }
    }

    void TransformService::transform(TransformComponent *st, const TransformComponent *parentTransform) {
        if (parentTransform != nullptr) {
            auxMat4 = parentTransform->model;
        } else {
            auxMat4 = glm::identity<glm::mat4>();
        }
        if (!st->forceTransform && st->isStatic) {
            LOG_WARN("Entity will not be transformed because it is set to static " + std::to_string(entt::to_integral(st->getEntityId())));
            return;
        }

        auxMat42 = glm::identity<glm::mat4>();
        auxMat42 = glm::translate(auxMat42, st->translation); // Translation

        auxMat42 = glm::rotate(auxMat42, glm::radians(st->rotationEuler.x), {1, 0, 0});
        auxMat42 = glm::rotate(auxMat42, glm::radians(st->rotationEuler.y), {0, 1, 0});
        auxMat42 = glm::rotate(auxMat42, glm::radians(st->rotationEuler.z), {0, 0, 1});

        auxMat42 = glm::scale(auxMat42, st->scale); // Scale

        st->model = auxMat4 * auxMat42;
        st->freezeVersion();

        if (worldRepository->hasComponent(st->getEntityId(), PRIMITIVE) || worldRepository->hasComponent(st->getEntityId(), LIGHT)) {
            rayTracingService->markDirty();
        }
    }
} // Metal
