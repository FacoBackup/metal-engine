#include <engine/service/TransformService.h>
#include <entt/entt.hpp>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <common/LoggerUtil.h>

#include <ApplicationContext.h>
#include <engine/dto/TransformComponent.h>
#include <engine/repository/WorldRepository.h>
#include <engine/dto/PrimitiveComponent.h>
#include <engine/service/RayTracingService.h>
#include <editor/dto/FieldModificationEvent.h>
#include <ApplicationEventContext.h>

namespace Metal {
    void TransformService::onInitialize() {
        eventListener([this](const Event &e) {
            const auto payload = std::static_pointer_cast<InspectableEventPayload>(e.payload);
            if (const auto transform = dynamic_cast<TransformComponent *>(payload->inspectable)) {
                worldRepository->dirtyEntities.insert(transform->getEntityId());
            }
        }, "TransformComponent");

        for (auto entity: worldRepository->registry.view<TransformComponent>()) {
            worldRepository->dirtyEntities.insert(entity);
        }
    }

    void TransformService::onAsyncSync() {
        if (worldRepository->dirtyEntities.empty()) return;

        for (auto entity: worldRepository->dirtyEntities) {
            if (!worldRepository->registry.valid(entity)) continue;
            TransformComponent &st = worldRepository->registry.get<TransformComponent>(entity);
            transform(&st, nullptr);
        }
        // NOTE: dirtyEntities are NOT cleared here.
        // TLASService will clear them after it's done using them.
        // Actually, if we have multiple services needing dirty flags,
        // we might need a better system, but for now we follow Phase 4.
    }

    void TransformService::transform(TransformComponent *st, const TransformComponent *parentTransform) {
        if (parentTransform != nullptr) {
            auxMat4 = parentTransform->model;
        } else {
            auxMat4 = glm::identity<glm::mat4>();
        }
        if (!st->forceTransform && st->isStatic) {
            LOG_WARN(
                "Entity will not be transformed because it is set to static " + std::to_string(entt::to_integral(st->
                    getEntityId())));
            return;
        }

        auxMat42 = glm::identity<glm::mat4>();
        auxMat42 = glm::translate(auxMat42, st->translation); // Translation

        auxMat42 = auxMat42 * glm::mat4_cast(st->rotation);

        auxMat42 = glm::scale(auxMat42, st->scale); // Scale

        st->model = auxMat4 * auxMat42;

        if (worldRepository->hasComponent(st->getEntityId(), PRIMITIVE) || worldRepository->hasComponent(
                st->getEntityId(), LIGHT)) {
            ApplicationEventContext::dispatch("BVHNeedsUpdate");
        }
    }
} // Metal
