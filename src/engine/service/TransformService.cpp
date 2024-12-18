#include "TransformService.h"

#include <iostream>

#include "../../context/ApplicationContext.h"
#include "world/components/TransformComponent.h"

namespace Metal {
    void TransformService::onSync() {
        traverse(WorldRepository::ROOT_ID, false);
    }

    void TransformService::traverse(const EntityID entityId, bool parentHasChanged) {
        if (TransformComponent *st = context.getEngineContext().worldRepository.transforms.at(entityId);
            st != nullptr && (st->isNotFrozen() || parentHasChanged)) {
            TransformComponent *parentTransform = findParent(st->getEntityId());
            transform(st, parentTransform);
            st->freezeVersion();
            parentHasChanged = true;
        }

        for (auto child: context.getEngineContext().worldRepository.entities.at(entityId)->children) {
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
            std::cout << "Entity will not be transformed because it is set to static " << st->getEntityId() <<
                    std::endl;
            return;
        }
        translation = glm::vec3(st->model[3]);
        auto *previousTile = context.getEngineContext().worldGridRepository.getOrCreateTile(translation);

        auxMat42 = glm::identity<glm::mat4>();
        auxMat42 = glm::translate(auxMat42, st->translation); // Translation
        auxMat42 *= glm::mat4(st->rotation);
        auxMat42 = glm::scale(auxMat42, st->scale); // Scale

        glm::mat4 auxMat4 = auxMat4 * auxMat42;

        st->model = auxMat42;
        st->freezeVersion();

        translation = glm::vec3(st->model[3]);
        auto *newTile = context.getEngineContext().worldGridRepository.getOrCreateTile(translation);

        context.getEngineContext().worldGridRepository.moveBetweenTiles(st->getEntityId(), previousTile, newTile);
    }

    TransformComponent *TransformService::findParent(EntityID id) const {
        while (id != EMPTY_ENTITY && id != WorldRepository::ROOT_ID) {
            id = context.getEngineContext().worldRepository.getEntity(id)->parent;
            if (auto *t = context.getEngineContext().worldRepository.transforms.at(id); t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    float TransformService::getDistanceFromCamera(glm::vec3 &translation) {
        distanceAux = context.getEngineContext().worldRepository.camera.position;
        return glm::length(distanceAux - translation);
    }
} // Metal
