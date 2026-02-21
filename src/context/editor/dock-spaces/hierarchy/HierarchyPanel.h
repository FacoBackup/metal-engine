#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H
#include <imgui.h>
#include <unordered_map>

#include "../../../../enum/engine-definitions.h"
#include "../../../../enum/ComponentType.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    struct EditorRepository;
    struct WorldRepository;
    struct AbstractComponent;
    struct EntityComponent;
    struct HierarchyComponent;
    class HierarchyHeaderPanel;

    class HierarchyPanel final : public AbstractDockPanel {
        static constexpr auto TRANSPARENT = ImVec4(0, 0, 0, 0);
        static constexpr auto PADDING = ImVec2(0, 0);
        static constexpr auto TABLE_FLAGS = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg
                                            | ImGuiTableFlags_NoBordersInBody;
        HierarchyHeaderPanel *headerPanel = nullptr;
        ImVec4 rowColor = ImVec4(0, 0, 0, 1);
        EntityID onDrag = EMPTY_ENTITY;
        bool isOnSearch = false;
        std::unordered_map<EntityID, std::string> searchMatchWith{};
        std::unordered_map<EntityID, bool> searchMatch{};
        std::unordered_map<EntityID, bool> opened{};
        WorldRepository *world = nullptr;
        EditorRepository *editorRepository = nullptr;
        bool isSomethingHovered = false;

    public:
        void onInitialize() override;

        void contextMenu() const;

        void onSync() override;

        bool renderNode(EntityID entityId);

        bool isOpen(EntityID entityId, int flags) const;

        const char *GetIcon(EntityID entityId) const;

        std::string getNodeLabel(EntityID entityId, bool addId) const;

        bool matchSearch(EntityID entityId);

        void renderEntityChildren(EntityID entityId);

        int getFlags(EntityID entityId);

        void renderEntityColumns(EntityID entityId, bool isPinned) const;

        void handleClick(EntityID entityId) const;

        void handleDragDrop(EntityID entityId);
    };
} // Metal

#endif //HIERARCHYPANEL_H
