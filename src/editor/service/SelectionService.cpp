#include "SelectionService.h"

#include "../../ApplicationContext.h"
#include "../../engine/dto/TransformComponent.h"

namespace Metal {
    void SelectionService::addSelected(entt::entity entity) const {
        auto &editorRepository = CTX.editorRepository;
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
        auto &editorRepository = CTX.editorRepository;
        editorRepository.selected.clear();
        editorRepository.mainSelection = EMPTY_ENTITY;
        editorRepository.primitiveSelected = nullptr;
    }

    void SelectionService::addAllSelected(const std::vector<entt::entity> &all) const {
        auto &editorRepository = CTX.editorRepository;
        editorRepository.selected.clear();
        const entt::entity first = all.size() > 0 ? all[0] : EMPTY_ENTITY;
        editorRepository.mainSelection = first;
        updatePrimitiveSelected();
        for (auto a: all) {
            editorRepository.selected.insert({a, true});
        }
    }

    void SelectionService::updatePrimitiveSelected() const {
        auto &editorRepository = CTX.editorRepository;
        auto &repo = CTX.worldRepository;
        const auto entityId = editorRepository.mainSelection;

        if (entityId == EMPTY_ENTITY || !repo.registry.valid(entityId)) {
            return;
        }

        if (repo.registry.all_of<TransformComponent>(entityId)) {
            editorRepository.primitiveSelected = &repo.registry.get<TransformComponent>(entityId);
        }
    }

} // Metal
