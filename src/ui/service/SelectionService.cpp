#include "SelectionService.h"

#include "../../MetalContextProvider.h"
#include "../../runtime/data/components/TransformComponent.h"

namespace Metal
{
    void SelectionService::addSelected(EntityID entity) const
    {
        if (editorRepository.selected.empty() || entity == EMPTY_ENTITY)
        {
            editorRepository.mainSelection = entity;
            if (editorRepository.mainSelection == WorldRepository::ROOT_ID)
            {
                editorRepository.mainSelection = EMPTY_ENTITY;
            }
            else if (editorRepository.mainSelection != EMPTY_ENTITY)
            {
                updatePrimitiveSelected();
            }
        }
        if (entity != EMPTY_ENTITY)
        {
            editorRepository.selected.insert({entity, true});
        }
    }

    void SelectionService::clearSelection() const
    {
        editorRepository.selected.clear();
        editorRepository.mainSelection = EMPTY_ENTITY;
        editorRepository.primitiveSelected = nullptr;
    }

    void SelectionService::addAllSelected(const std::vector<EntityID>& all) const
    {
        editorRepository.selected.clear();
        const EntityID first = all.size() > 0 ? all[0] : EMPTY_ENTITY;
        editorRepository.mainSelection = first;
        updatePrimitiveSelected();
        for (auto a : all)
        {
            editorRepository.selected.insert({a, true});
        }
    }

    void SelectionService::updatePrimitiveSelected() const
    {
        auto& comp = world.getEntity(editorRepository.mainSelection)->components;
        for (auto a : comp)
        {
            if (a == ComponentTypes::TRANSFORM)
            {
                editorRepository.primitiveSelected = dynamic_cast<TransformComponent*>(SINGLETONS.
                    worldRepository.getComponent(ComponentTypes::TRANSFORM, editorRepository.mainSelection));
                break;
            }
        }
    }

    SelectionService::SelectionService():
        editorRepository(SINGLETONS.editorRepository),
        world(SINGLETONS.worldRepository)
    {
    }
} // Metal
