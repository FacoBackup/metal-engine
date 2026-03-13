#include "SelectionService.h"
#include "../../editor/repository/EditorRepository.h"
#include "../../engine/repository/WorldRepository.h"
#include "../../engine/dto/TransformComponent.h"

namespace Metal {
    void SelectionService::addSelected(entt::entity entity) const {
        if (editorRepository.selected.empty() || entity == EMPTY_ENTITY) {
            editorRepository.mainSelection = entity;
            if (editorRepository.mainSelection != EMPTY_ENTITY) {
                updatePrimitiveSelected();
            }
        }
        if (entity != EMPTY_ENTITY) {
            editorRepository.selected.insert({entity, true});
        }
    }

    void SelectionService::clearSelection() const {
        editorRepository.selected.clear();
        editorRepository.mainSelection = EMPTY_ENTITY;
        editorRepository.primitiveSelected = nullptr;
    }

    void SelectionService::addAllSelected(const std::vector<entt::entity> &all) const {
        editorRepository.selected.clear();
        const entt::entity first = all.size() > 0 ? all[0] : EMPTY_ENTITY;
        editorRepository.mainSelection = first;
        updatePrimitiveSelected();
        for (auto a: all) {
            editorRepository.selected.insert({a, true});
        }
    }

    void SelectionService::updatePrimitiveSelected() const {
        const auto entityId = editorRepository.mainSelection;

        if (entityId == EMPTY_ENTITY || !worldRepository.registry.valid(entityId)) {
            return;
        }

        if (worldRepository.registry.all_of<TransformComponent>(entityId)) {
            editorRepository.primitiveSelected = &worldRepository.registry.get<TransformComponent>(entityId);
        }
    }

} // Metal
