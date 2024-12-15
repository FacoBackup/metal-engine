#include "SelectionService.h"

#include "../../context/ApplicationContext.h"
#include "../../engine/service/world/impl/TransformComponent.h"

namespace Metal {
    void SelectionService::addSelected(EntityID entity) const {
        if (editorRepository.selected.empty() || entity == nullptr) {
            editorRepository.mainSelection = entity;
            if (editorRepository.mainSelection != nullptr && strcmp(editorRepository.mainSelection,
                                                                    WorldRepository::ROOT_ID.c_str())) {
                editorRepository.mainSelection = nullptr;
            } else if (editorRepository.mainSelection != nullptr) {
                updatePrimitiveSelected();
            }
        }
        if (entity != nullptr) {
            editorRepository.selected.insert({entity, true});
        }
    }

    void SelectionService::clearSelection() const {
        editorRepository.selected.clear();
        editorRepository.mainSelection = nullptr;
        editorRepository.primitiveSelected = nullptr;
    }

    void SelectionService::addAllSelected(std::vector<EntityID> all) const {
        editorRepository.selected.clear();
        const EntityID first = all.size() > 0 ? all[0] : nullptr;
        editorRepository.mainSelection = first;
        updatePrimitiveSelected();
        for (auto a: all) {
            editorRepository.selected.insert({a, true});
        }
    }

    void SelectionService::updatePrimitiveSelected() const {
        // auto &comp = world.entities[editorRepository.mainSelection].components;
        // editorRepository.primitiveSelected = comp.contains(ComponentTypes::ComponentType::TRANSFORM)
        //                                          ? static_cast<TransformComponent*>(&comp[ComponentTypes::ComponentType::TRANSFORM])
        //                                          : nullptr;
    }

    SelectionService::SelectionService(ApplicationContext &context)
        : AbstractRuntimeComponent(context),
          editorRepository(context.getEditorContext().editorRepository),
          world(context.getEngineContext().worldRepository) {
    }
} // Metal
