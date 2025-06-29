#include "TransformService.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>

#include "../../MetalContextProvider.h"
#include "../data/components/TransformComponent.h"

namespace Metal {
    void TransformService::onSync() {
        traverse(WorldRepository::ROOT_ID, false);
        if (somethingChanged) {
            SINGLETONS.engineContext.dispatchTLASUpdate();
            somethingChanged = false;
        }
    }

    void TransformService::traverse(const EntityID entityId, bool parentHasChanged) {
        TransformComponent *st = SINGLETONS.worldRepository.transforms.contains(entityId)
                                     ? &SINGLETONS.worldRepository.transforms.at(entityId)
                                     : nullptr;
        if (st != nullptr && (st->isNotFrozen() || parentHasChanged)) {
            TransformComponent *parentTransform = findParent(st->getEntityId());
            transform(st, parentTransform);
            st->freezeVersion();
            parentHasChanged = true;
        }

        for (auto child: SINGLETONS.worldRepository.getEntity(entityId)->children) {
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

        auxMat42 = glm::identity<glm::mat4>();
        auxMat42 = glm::translate(auxMat42, st->translation); // Translation
        auxMat42 *= glm::mat4_cast(st->rotation);
        auxMat42 = glm::scale(auxMat42, st->scale); // Scale

        st->model = auxMat4 * auxMat42;
        st->freezeVersion();

        translation = glm::vec3(st->model[3]);
        somethingChanged = SINGLETONS.worldRepository.meshes.contains(st->getEntityId());
    }

    TransformComponent *TransformService::findParent(EntityID id) const {
        while (id != EMPTY_ENTITY && id != WorldRepository::ROOT_ID) {
            id = SINGLETONS.worldRepository.getEntity(id)->parent;
            TransformComponent *t = SINGLETONS.worldRepository.transforms.contains(id)
                                        ? &SINGLETONS.worldRepository.transforms.at(id)
                                        : nullptr;
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    float TransformService::getDistanceFromCamera(glm::vec3 &translation) {
        distanceAux = SINGLETONS.worldRepository.camera.position;
        return glm::length(distanceAux - translation);
    }
} // Metal
