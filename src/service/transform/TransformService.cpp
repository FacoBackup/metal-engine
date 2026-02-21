#include "TransformService.h"
#include <entt/entt.hpp>
#include <iostream>
#include <glm/gtc/quaternion.hpp>

#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/TransformComponent.h"

namespace Metal {
    void TransformService::onSync() {
        traverse(WorldRepository::ROOT_ID, false);
    }

    void TransformService::traverse(const EntityID entityId, bool parentHasChanged) {
        const auto entity = static_cast<entt::entity>(entityId);
        TransformComponent *st = CTX.worldRepository.registry.all_of<TransformComponent>(entity)
                                     ? &CTX.worldRepository.registry.get<TransformComponent>(entity)
                                     : nullptr;
        if (st != nullptr && (st->isNotFrozen() || parentHasChanged)) {
            TransformComponent *parentTransform = findParent(st->getEntityId());
            transform(st, parentTransform);
            st->freezeVersion();
            parentHasChanged = true;
        }

        const auto e = static_cast<entt::entity>(entityId);
        if (CTX.worldRepository.registry.all_of<HierarchyComponent>(e)) {
            const auto &hierarchy = CTX.worldRepository.registry.get<HierarchyComponent>(e);
            for (auto child: hierarchy.children) {
                traverse(child, parentHasChanged);
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
            LOG_WARN("Entity will not be transformed because it is set to static " + std::to_string(st->getEntityId()));
            return;
        }
        translation = glm::vec3(st->model[3]);
        auto *previousTile = CTX.worldGridRepository.getOrCreateTile(translation);

        auxMat42 = glm::identity<glm::mat4>();
        auxMat42 = glm::translate(auxMat42, st->translation); // Translation
        auxMat42 *= glm::mat4_cast(st->rotation);
        auxMat42 = glm::scale(auxMat42, st->scale); // Scale

        st->model = auxMat4 * auxMat42;
        st->freezeVersion();

        translation = glm::vec3(st->model[3]);
        auto *newTile = CTX.worldGridRepository.getOrCreateTile(translation);

        CTX.worldGridRepository.moveBetweenTiles(st->getEntityId(), previousTile, newTile);
        CTX.rayTracingService.markDirty();
    }

    TransformComponent *TransformService::findParent(EntityID id) const {
        while (id != EMPTY_ENTITY && id != WorldRepository::ROOT_ID) {
            const auto e = static_cast<entt::entity>(id);
            if (!CTX.worldRepository.registry.valid(e)) break;
            const auto &hierarchy = CTX.worldRepository.registry.get<HierarchyComponent>(e);
            id = hierarchy.parent;
            const auto parent = static_cast<entt::entity>(id);
            TransformComponent *t = (CTX.worldRepository.registry.valid(parent) && CTX.worldRepository.registry.all_of<TransformComponent>(parent))
                          ? &CTX.worldRepository.registry.get<TransformComponent>(parent)
                          : nullptr;
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    float TransformService::getDistanceFromCamera(glm::vec3 &translation) {
        distanceAux = CTX.worldRepository.camera.position;
        return glm::length(distanceAux - translation);
    }
} // Metal
