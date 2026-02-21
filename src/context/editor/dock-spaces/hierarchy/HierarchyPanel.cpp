#include "HierarchyPanel.h"

#include "HierarchyHeaderPanel.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/impl/EntityComponent.h"
#include "../../../../enum/ComponentType.h"

namespace Metal {
    void HierarchyPanel::onInitialize() {
        appendChild(headerPanel = new HierarchyHeaderPanel());
        world = &CTX.worldRepository;
        editorRepository = &CTX.editorRepository;

        shortcuts = {
            ShortcutDTO("Delete", ImGuiKey_Delete, [this]() {
                std::vector<EntityID> entities;
                for (auto &entry: CTX.editorRepository.selected) {
                    entities.push_back(entry.first);
                }
                CTX.worldRepository.deleteEntities(entities);
                CTX.selectionService.clearSelection();
            }),
            ShortcutDTO("Select All", ImGuiMod_Ctrl | ImGuiKey_A, [this]() {
                std::vector<EntityID> entities;
                auto& storage = world->registry.storage<entt::entity>();
                for (auto it = storage.begin(); it != storage.end(); ++it) {
                    auto entity = *it;
                    if (static_cast<EntityID>(entity) != WorldRepository::ROOT_ID && world->registry.all_of<EntityComponent>(entity)) {
                        entities.push_back(static_cast<EntityID>(entity));
                    }
                }
                CTX.selectionService.addAllSelected(entities);
            })
        };
    }

    void HierarchyPanel::contextMenu() const {
        if (ImGui::BeginPopupContextItem((id + "contextMenu").c_str())) {
            if (ImGui::MenuItem("Delete")) {
                std::vector<EntityID> entities;
                for (auto &entry: CTX.editorRepository.selected) {
                    entities.push_back(entry.first);
                }
                CTX.worldRepository.deleteEntities(entities);
                CTX.selectionService.clearSelection();
            }
            ImGui::EndPopup();
        }

        if (isSomethingHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup((id + "contextMenu").c_str());
        }
    }

    void HierarchyPanel::onSync() {
        isOnSearch = strlen(headerPanel->search) > 0;

        isSomethingHovered = ImGui::IsItemHovered();

        // hotKeys();
        onSyncChildren();
        ImGui::Separator();
        if (ImGui::BeginTable((id + "hierarchyTable").c_str(), 2, TABLE_FLAGS)) {
            isSomethingHovered = isSomethingHovered || ImGui::IsItemHovered();
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn(Icons::visibility.c_str(), ImGuiTableColumnFlags_WidthFixed, 20.f);
            ImGui::TableHeadersRow();
            renderNode(WorldRepository::ROOT_ID);
            ImGui::EndTable();
        }
        if (!CTX.editorRepository.selected.empty()) {
            contextMenu();
        }
    }

    bool HierarchyPanel::renderNode(const EntityID entityId) {
        EntityComponent *node = world->getEntity(entityId);
        if (node == nullptr || (isOnSearch &&
                                searchMatch.contains(entityId) &&
                                searchMatchWith.contains(entityId) &&
                                strcmp(searchMatchWith[entityId].c_str(), headerPanel->search) == 0)) {
            return false;
        }

        const bool isSearchMatch = matchSearch(entityId);
        ImGui::TableNextRow();
        if (editorRepository->selected.contains(entityId)) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, editorRepository->accentU32);
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, editorRepository->accentU32);
        }
        ImGui::TableNextColumn();
        const int flags = getFlags(entityId);
        bool open;

        if (world->culled.contains(entityId) || world->hiddenEntities.contains(entityId)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(node->color.x, node->color.y, node->color.z, .5));
            open = isOpen(entityId, flags);
            ImGui::PopStyleColor();
        } else {
            rowColor.x = node->color.x;
            rowColor.y = node->color.y;
            rowColor.z = node->color.z;
            ImGui::PushStyleColor(ImGuiCol_Text, rowColor);
            open = isOpen(entityId, flags);
            ImGui::PopStyleColor();
        }

        if (entityId != WorldRepository::ROOT_ID) {
            handleDragDrop(entityId);
            renderEntityColumns(entityId, false);
        }

        if (open) {
            opened.insert({entityId, ImGuiTreeNodeFlags_DefaultOpen});
            renderEntityChildren(entityId);
        } else {
            opened.insert({entityId, ImGuiTreeNodeFlags_None});
        }

        return isSearchMatch;
    }

    bool HierarchyPanel::isOpen(const EntityID entityId, const int flags) const {
        return ImGui::TreeNodeEx(getNodeLabel(entityId, true).c_str(), flags);
    }

    const char *HierarchyPanel::GetIcon(const EntityID entityId) const {
        const auto entity = static_cast<entt::entity>(entityId);
        if (world->registry.all_of<MeshComponent>(entity)) {
            return ComponentTypes::IconOf(ComponentTypes::MESH);
        }
        if (world->registry.all_of<std::unique_ptr<LightComponent>>(entity)) {
            return ComponentTypes::IconOf(ComponentTypes::SPHERE_LIGHT);
        }
        if (world->registry.all_of<VolumeComponent>(entity)) {
            return ComponentTypes::IconOf(ComponentTypes::VOLUME);
        }
        return Icons::inventory_2.c_str();
    }

    std::string HierarchyPanel::getNodeLabel(const EntityID entityId, const bool addId) const {
        const auto node = world->getEntity(entityId);
        return std::format("{}{}##{}{}",
                           GetIcon(entityId),
                           node->name,
                           entityId,
                           addId ? id : "");
    }

    bool HierarchyPanel::matchSearch(const EntityID entityId) {
        bool isSearchMatch = false;
        const auto node = world->getEntity(entityId);
        if (isOnSearch) {
            isSearchMatch = node->name.find(headerPanel->search) != std::string::npos;
            if (isSearchMatch) {
                searchMatch.insert({entityId, true});
            } else {
                searchMatch.erase(entityId);
            }
            searchMatchWith.insert({entityId, headerPanel->search});
        } else {
            searchMatch.erase(entityId);
            searchMatchWith.erase(entityId);
        }
        return isSearchMatch;
    }

    void HierarchyPanel::renderEntityChildren(const EntityID entityId) {
        const auto entity = static_cast<entt::entity>(entityId);
        if (!world->registry.all_of<HierarchyComponent>(entity)) {
            ImGui::TreePop();
            return;
        }

        const auto &hierarchy = world->registry.get<HierarchyComponent>(entity);
        if (isOnSearch) {
            for (const auto child: hierarchy.children) {
                if (searchMatch.contains(entityId) || renderNode(child)) {
                    searchMatch.insert({entityId, true});
                } else {
                    searchMatch.erase(entityId);
                }
            }
        } else {
            for (const auto child: hierarchy.children) {
                renderNode(child);
            }
        }

        ImGui::TreePop();
    }

    int HierarchyPanel::getFlags(const EntityID entityId) {
        int flags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (isOnSearch) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (opened.contains(entityId)) {
            flags |= opened[entityId];
        }
        return flags;
    }

    void HierarchyPanel::renderEntityColumns(const EntityID entityId, const bool isPinned) const {
        const auto idString = std::to_string(entityId);
        handleClick(entityId);
        ImGui::TableNextColumn();

        ImGui::PushStyleColor(ImGuiCol_Button, TRANSPARENT);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

        bool isVisible = !world->hiddenEntities.contains(entityId);
        if (UIUtil::ButtonSimple(
            (isVisible ? Icons::visibility : Icons::visibility_off) + (isPinned ? "##vpinned" : "##v") + idString +
            id, 20, 15)) {
            CTX.worldRepository.changeVisibility(entityId, !isVisible);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    void HierarchyPanel::handleClick(const EntityID entityId) const {
        if (ImGui::IsItemClicked()) {
            if (const bool isMultiSelect = ImGui::IsKeyDown(ImGuiKey_LeftCtrl); !isMultiSelect) {
                CTX.selectionService.clearSelection();
            }
            CTX.selectionService.addSelected(entityId);
        }
    }

    void HierarchyPanel::handleDragDrop(const EntityID entityId) {
        const auto node = world->getEntity(entityId);
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(id.c_str(), id.c_str(), sizeof(id.c_str()));
            onDrag = entityId;
            ImGui::Text("Dragging Node %s", node->getTitle());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (strcmp(static_cast<const char *>(ImGui::AcceptDragDropPayload(id.c_str())->Data), id.c_str())) {
                world->linkEntities(entityId, onDrag);
            }
            ImGui::EndDragDropTarget();
        }
    }
} // Metal
