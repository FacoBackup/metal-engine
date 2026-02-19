#include "TransformService.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>

#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/TransformComponent.h"

namespace Metal {
    void TransformService::onSync() {
        traverse(WorldRepository::ROOT_ID, false);
    }

    void TransformService::traverse(const EntityID entityId, bool parentHasChanged) {
        TransformComponent *st = ApplicationContext::Get().worldRepository.transforms.contains(entityId)
                                     ? &ApplicationContext::Get().worldRepository.transforms.at(entityId)
                                     : nullptr;
        if (st != nullptr && (st->isNotFrozen() || parentHasChanged)) {
            TransformComponent *parentTransform = findParent(st->getEntityId());
            transform(st, parentTransform);
            st->freezeVersion();
            parentHasChanged = true;
        }

        for (auto child: ApplicationContext::Get().worldRepository.getEntity(entityId)->children) {
            traverse(child, parentHasChanged);
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
        auto *previousTile = ApplicationContext::Get().worldGridRepository.getOrCreateTile(translation);

        auxMat42 = glm::identity<glm::mat4>();
        auxMat42 = glm::translate(auxMat42, st->translation); // Translation
        auxMat42 *= glm::mat4_cast(st->rotation);
        auxMat42 = glm::scale(auxMat42, st->scale); // Scale

        st->model = auxMat4 * auxMat42;
        st->freezeVersion();

        translation = glm::vec3(st->model[3]);
        auto *newTile = ApplicationContext::Get().worldGridRepository.getOrCreateTile(translation);

        ApplicationContext::Get().worldGridRepository.moveBetweenTiles(st->getEntityId(), previousTile, newTile);
    }

    TransformComponent *TransformService::findParent(EntityID id) const {
        while (id != EMPTY_ENTITY && id != WorldRepository::ROOT_ID) {
            id = ApplicationContext::Get().worldRepository.getEntity(id)->parent;
            TransformComponent *t = ApplicationContext::Get().worldRepository.transforms.contains(id)
                          ? &ApplicationContext::Get().worldRepository.transforms.at(id)
                          : nullptr;
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    float TransformService::getDistanceFromCamera(glm::vec3 &translation) {
        distanceAux = ApplicationContext::Get().worldRepository.camera.position;
        return glm::length(distanceAux - translation);
    }
} // Metal
