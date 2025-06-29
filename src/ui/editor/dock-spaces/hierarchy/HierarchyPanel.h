#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H
#include <imgui.h>
#include <unordered_map>

#include "../../../../common/engine-definitions.h"
#include "../../../../common/enum/ComponentType.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    struct EditorRepository;
    struct WorldRepository;
    struct AbstractComponent;
    class Entity;
    class HierarchyHeaderPanel;

    class HierarchyPanel final : public AbstractDockPanel {
        char searchBuffer[128] = "";
        EntityID dragSource = EMPTY_ENTITY;

        static const char *GetIcon(const Entity *node);

        bool MatchesSearchRecursive(Entity *entity);

        void handleDrop(EntityID entityID, Entity *entity);

        void handleDrag(EntityID entityID, Entity *entity);

        bool drawContextMenu(EntityID entityID, Entity *entity, bool isHidden);

        void handleSelection(EntityID entityID);

        void renderHideButton(EntityID entityID, bool isHidden);

        void DrawEntityNode(EntityID entityID);

    public:
        void onSync() override;
    };
} // Metal

#endif //HIERARCHYPANEL_H
