#include "HierarchyPanel.h"

#include "HierarchyHeaderPanel.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/impl/Entity.h"
#include "../../../../enum/ComponentType.h"

namespace Metal {
    void HierarchyPanel::onInitialize() {
        appendChild(headerPanel = new HierarchyHeaderPanel());
        world = &context->worldRepository;
        editorRepository = &context->editorRepository;
    }

    void HierarchyPanel::contextMenu() const {
        if (ImGui::BeginPopupContextItem((id + "contextMenu").c_str())) {
            if (ImGui::MenuItem("Delete")) {
                std::vector<EntityID> entities;
                for (auto &entry: context->editorRepository.selected) {
                    entities.push_back(entry.first);
                }
                context->worldRepository.deleteEntities(entities);
                context->selectionService.clearSelection();
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
        if (!context->editorRepository.selected.empty()) {
            contextMenu();
        }
    }

    bool HierarchyPanel::renderNode(const EntityID entityId) {
        Entity *node = world->getEntity(entityId);
        if (node == nullptr || (isOnSearch &&
                                searchMatch.contains(node->getId()) &&
                                searchMatchWith.contains(node->getId()) &&
                                strcmp(searchMatchWith[node->getId()].c_str(), headerPanel->search))) {
            return false;
        }

        const bool isSearchMatch = matchSearch(node);
        ImGui::TableNextRow();
        if (editorRepository->selected.contains(entityId)) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, editorRepository->accentU32);
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, editorRepository->accentU32);
        }
        ImGui::TableNextColumn();
        const int flags = getFlags(node);
        bool open;

        if (world->culled.contains(entityId) || world->hiddenEntities.contains(entityId)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(node->color.x, node->color.y, node->color.z, .5));
            open = isOpen(node, flags);
            ImGui::PopStyleColor();
        } else {
            rowColor.x = node->color.x;
            rowColor.y = node->color.y;
            rowColor.z = node->color.z;
            ImGui::PushStyleColor(ImGuiCol_Text, rowColor);
            open = isOpen(node, flags);
            ImGui::PopStyleColor();
        }

        if (node->getId() != WorldRepository::ROOT_ID) {
            handleDragDrop(node);
            renderEntityColumns(node, false);
        }

        if (open) {
            opened.insert({node->getId(), ImGuiTreeNodeFlags_DefaultOpen});
            renderEntityChildren(node);
        } else {
            opened.insert({node->getId(), ImGuiTreeNodeFlags_None});
        }

        return isSearchMatch;
    }

    bool HierarchyPanel::isOpen(Entity *node, const int flags) const {
        return ImGui::TreeNodeEx(getNodeLabel(node, true).c_str(), flags);
    }

    const char *HierarchyPanel::GetIcon(const Metal::Entity *node) {
        for (const auto comp: node->components) {
            if (comp == ComponentTypes::MESH) {
                return Icons::view_in_ar.c_str();
            }

            if (comp == ComponentTypes::LIGHT) {
                return Icons::lightbulb.c_str();
            }
        }
        return Icons::inventory_2.c_str();
    }

    std::string HierarchyPanel::getNodeLabel(Entity *node, const bool addId) const {
        return std::format("{}{}##{}{}",
                           GetIcon(node),
                           node->name,
                           node->getId(),
                           addId ? id : "");
    }

    bool HierarchyPanel::matchSearch(const Entity *node) {
        bool isSearchMatch = false;
        if (isOnSearch) {
            isSearchMatch = node->name.find(headerPanel->search) != std::string::npos;
            if (isSearchMatch) {
                searchMatch.insert({node->getId(), true});
            } else {
                searchMatch.erase(node->getId());
            }
            searchMatchWith.insert({node->getId(), headerPanel->search});
        } else {
            searchMatch.erase(node->getId());
            searchMatchWith.erase(node->getId());
        }
        return isSearchMatch;
    }

    void HierarchyPanel::renderEntityChildren(const Entity *node) {
        if (isOnSearch) {
            for (const auto child: node->children) {
                if (searchMatch.contains(node->getId()) || renderNode(child)) {
                    searchMatch.insert({node->getId(), true});
                } else {
                    searchMatch.erase(node->getId());
                }
            }
        } else {
            for (const auto child: node->children) {
                renderNode(child);
            }
        }

        ImGui::TreePop();
    }

    int HierarchyPanel::getFlags(const Entity *node) {
        int flags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (isOnSearch) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (opened.contains(node->getId())) {
            flags |= opened[node->getId()];
        }
        return flags;
    }

    void HierarchyPanel::renderEntityColumns(const Entity *node, const bool isPinned) const {
        const auto idString = std::to_string(node->getId());
        handleClick(node);
        ImGui::TableNextColumn();

        ImGui::PushStyleColor(ImGuiCol_Button, TRANSPARENT);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

        bool isVisible = !world->hiddenEntities.contains(node->getId());
        if (UIUtil::ButtonSimple(
            (isVisible ? Icons::visibility : Icons::visibility_off) + (isPinned ? "##vpinned" : "##v") + idString +
            id, 20, 15)) {
            context->worldRepository.changeVisibility(node->getId(), !isVisible);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    void HierarchyPanel::handleClick(const Entity *node) const {
        if (ImGui::IsItemClicked()) {
            if (const bool isMultiSelect = ImGui::IsKeyDown(ImGuiKey_LeftCtrl); !isMultiSelect) {
                context->selectionService.clearSelection();
            }
            context->selectionService.addSelected(node->getId());
        }
    }

    void HierarchyPanel::handleDragDrop(Entity *node) {
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(id.c_str(), id.c_str(), sizeof(id.c_str()));
            onDrag = node;
            ImGui::Text("Dragging Node %s", node->getTitle());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (strcmp(static_cast<const char *>(ImGui::AcceptDragDropPayload(id.c_str())->Data), id.c_str())) {
                world->linkEntities(node, onDrag);
            }
            ImGui::EndDragDropTarget();
        }
    }
} // Metal
