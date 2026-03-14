#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H
#include <imgui.h>
#include <unordered_map>

#include "../../enum/engine-definitions.h"
#include "../../../engine/enum/ComponentType.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    struct EditorRepository;
    struct WorldRepository;
    struct AbstractComponent;
    struct MetadataComponent;
    class WorldHeaderPanel;
    class SelectionService;
    struct DirectoryService;

    class WorldPanel final : public AbstractDockPanel {
        static constexpr auto TRANSPARENT = ImVec4(0, 0, 0, 0);
        static constexpr auto PADDING = ImVec2(0, 0);
        static constexpr auto TABLE_FLAGS = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg
                                            | ImGuiTableFlags_NoBordersInBody;
        WorldHeaderPanel *headerPanel = nullptr;
        entt::entity onDrag = EMPTY_ENTITY;
        bool isOnSearch = false;
        WorldRepository *world = nullptr;
        EditorRepository *editorRepository = nullptr;
        bool isSomethingHovered = false;

        SelectionService *selectionService = nullptr;
        DirectoryService *directoryService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", world},
                {"EditorRepository", editorRepository},
                {"SelectionService", selectionService},
                {"DirectoryService", directoryService}
            };
        }

        void onInitialize() override;

        void contextMenu() const;

        void onSync() override;

        void renderNode(entt::entity entityId);

        bool processEntityNode(entt::entity entityId, MetadataComponent *node);

        void applyRowStyle(entt::entity entityId, MetadataComponent *node) const;

        std::string getNodeLabel(entt::entity entityId) const;

        bool isMatched(entt::entity entityId) const;

        void renderEntityChildren(entt::entity entityId);

        int getFlags(entt::entity entityId) const;

        void renderEntityColumns(entt::entity entityId) const;

        void handleClick(entt::entity entityId) const;

        void handleDragDrop(entt::entity entityId);
    };
} // Metal

#endif //HIERARCHYPANEL_H
