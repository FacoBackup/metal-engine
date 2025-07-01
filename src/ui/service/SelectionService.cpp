#include "SelectionService.h"

#include "../../MetalContextProvider.h"
#include "../../runtime/data/components/TransformComponent.h"

namespace Metal {
    void SelectionService::addSelected(EntityID entity) const {
        if (SINGLETONS.editorRepository.selected.empty() || entity == EMPTY_ENTITY) {
            SINGLETONS.editorRepository.mainSelection = entity;
            if (SINGLETONS.editorRepository.mainSelection == WorldRepository::ROOT_ID) {
                SINGLETONS.editorRepository.mainSelection = EMPTY_ENTITY;
            } else if (SINGLETONS.editorRepository.mainSelection != EMPTY_ENTITY) {
                updatePrimitiveSelected();
            }
        }
        if (entity != EMPTY_ENTITY) {
            SINGLETONS.editorRepository.selected.insert({entity, true});
        }
    }

    void SelectionService::clearSelection() const {
        SINGLETONS.editorRepository.selected.clear();
        SINGLETONS.editorRepository.mainSelection = EMPTY_ENTITY;
        SINGLETONS.editorRepository.primitiveSelected = nullptr;
    }

    void SelectionService::addAllSelected(const std::vector<EntityID> &all) const {
        SINGLETONS.editorRepository.selected.clear();
        const EntityID first = all.size() > 0 ? all[0] : EMPTY_ENTITY;
        SINGLETONS.editorRepository.mainSelection = first;
        updatePrimitiveSelected();
        for (auto a: all) {
            SINGLETONS.editorRepository.selected.insert({a, true});
        }
    }

    void SelectionService::updatePrimitiveSelected() const {
        auto &comp = SINGLETONS.worldRepository.getEntity(SINGLETONS.editorRepository.mainSelection)->components;
        for (auto a: comp) {
            if (a == ComponentTypes::TRANSFORM) {
                SINGLETONS.editorRepository.primitiveSelected = dynamic_cast<TransformComponent *>(SINGLETONS.
                    worldRepository.getComponent(ComponentTypes::TRANSFORM, SINGLETONS.editorRepository.mainSelection));
                break;
            }
        }
    }
} // Metal
