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
    class Entity;

    class HierarchyPanel final : public AbstractDockPanel {
        static constexpr auto TRANSPARENT = ImVec4(0, 0, 0, 0);
        static constexpr auto PADDING = ImVec2(0, 0);
        static constexpr auto TABLE_FLAGS = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg
                                            | ImGuiTableFlags_NoBordersInBody;

        char *search = "";
        ImVec4 rowColor = ImVec4(0, 0, 0, 1);
        Entity *onDrag = nullptr;
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

        bool isOpen(Entity *node, int flags) const;

        static const char* GetIcon(const Metal::Entity * node);

        std::string getNodeLabel(Entity *node, bool addId) const;

        bool matchSearch(const Entity *node);

        void renderEntityChildren(const Entity *node);

        int getFlags(const Entity *node);

        void renderEntityColumns(const Entity *node, bool isPinned) const;

        void handleClick(const Entity *node) const;

        void handleDragDrop(Entity *node);
    };
} // Metal

#endif //HIERARCHYPANEL_H
