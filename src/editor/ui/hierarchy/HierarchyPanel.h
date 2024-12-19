#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H
#include <imgui.h>
#include <unordered_map>

#include "../../../engine/engine-definitions.h"
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

    public:
        void onInitialize() override;

        void onSync() override;

        void renderNodePinned(Entity *node);

        bool renderNode(EntityID entityId);

        bool isOpen(Entity *node, int flags) const;

        std::string getNodeLabel(Entity *node, bool addId) const;

        bool matchSearch(const Entity *node);

        void renderEntityChildren(const Entity *node);

        void renderComponents(const Entity *node) const;

        static void addComponent(AbstractComponent *component);

        int getFlags(const Entity *node);

        void renderEntityColumns(const Entity *node, bool isPinned);

        void changeVisibilityRecursively(EntityID node, bool isVisible);

        void handleClick(const Entity *node) const;

        void handleDragDrop(Entity *node);
    };
} // Metal

#endif //HIERARCHYPANEL_H
