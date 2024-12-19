#include "SelectionService.h"

#include "../../context/ApplicationContext.h"
#include "../../engine/service/world/components/TransformComponent.h"

namespace Metal {
    void SelectionService::addSelected(EntityID entity) const {
        if (editorRepository.selected.empty() || entity == EMPTY_ENTITY) {
            editorRepository.mainSelection = entity;
            if (editorRepository.mainSelection == WorldRepository::ROOT_ID) {
                editorRepository.mainSelection = EMPTY_ENTITY;
            } else if (editorRepository.mainSelection != EMPTY_ENTITY) {
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

    void SelectionService::addAllSelected(const std::vector<EntityID> &all) const {
        editorRepository.selected.clear();
        const EntityID first = all.size() > 0 ? all[0] : EMPTY_ENTITY;
        editorRepository.mainSelection = first;
        updatePrimitiveSelected();
        for (auto a: all) {
            editorRepository.selected.insert({a, true});
        }
    }

    void SelectionService::updatePrimitiveSelected() const {
        auto &comp = world.getEntity(editorRepository.mainSelection)->components;
        editorRepository.primitiveSelected = comp.contains(ComponentTypes::ComponentType::TRANSFORM)
                                                 ? static_cast<TransformComponent *>(comp.find(
                                                     ComponentTypes::ComponentType::TRANSFORM)->second)
                                                 : nullptr;
    }

    SelectionService::SelectionService(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          editorRepository(context.getEditorContext().editorRepository),
          world(context.getEngineContext().worldRepository) {
    }
} // Metal
