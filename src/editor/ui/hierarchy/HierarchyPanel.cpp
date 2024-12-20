#include "HierarchyPanel.h"

#include "HierarchyHeaderPanel.h"
#include "../../../common/interface/Icons.h"
#include "../../../common/util/UIUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../../engine/service/world/impl/Entity.h"
#include "../../../engine/service/world/impl/AbstractComponent.h"

namespace Metal {
    void HierarchyPanel::onInitialize() {
        appendChild(new HierarchyHeaderPanel(search));
        world = &context->getEngineContext().worldRepository;
        editorRepository = &context->getEditorContext().editorRepository;
    }


    void HierarchyPanel::onSync() {
        isOnSearch = strlen(search) > 0;
        // hotKeys();
        onSyncChildren();
        ImGui::Separator();
        if (ImGui::BeginTable((id + "hierarchyTable").c_str(), 3, TABLE_FLAGS)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn(Icons::visibility.c_str(), ImGuiTableColumnFlags_WidthFixed, 20.f);
            ImGui::TableSetupColumn(Icons::lock.c_str(), ImGuiTableColumnFlags_WidthFixed, 20.f);
            ImGui::TableHeadersRow();

            for (const auto &key: editorRepository->pinnedEntities) {
                renderNodePinned(world->getEntity(key.first));
            }
            renderNode(WorldRepository::ROOT_ID);
            ImGui::EndTable();
        }
    }

    void HierarchyPanel::renderNodePinned(Entity *node) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (editorRepository->selected.contains(node->getId())) {
            ImGui::TextColored(editorRepository->accent, "%s", getNodeLabel(node, false).c_str());
        } else {
            ImGui::TextColored(ImVec4(node->color.x, node->color.y, node->color.z, 1), "%s",
                               getNodeLabel(node, false).c_str());
        }
        renderEntityColumns(node, true);
    }

    bool HierarchyPanel::renderNode(const EntityID entityId) {
        Entity *node = world->getEntity(entityId);
        if (node == nullptr || (isOnSearch &&
                                searchMatch.contains(node->getId()) &&
                                searchMatchWith.contains(node->getId()) &&
                                strcmp(searchMatchWith[node->getId()].c_str(), search))) {
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

    std::string HierarchyPanel::getNodeLabel(Entity *node, const bool addId) const {
        return std::format("{}{}##{}{}",
                           node->isContainer ? Icons::inventory_2 : Icons::view_in_ar,
                           node->name,
                           node->getId(),
                           addId ? id : "");
    }

    bool HierarchyPanel::matchSearch(const Entity *node) {
        bool isSearchMatch = false;
        if (isOnSearch) {
            isSearchMatch = node->name.find(search) != std::string::npos;
            if (isSearchMatch) {
                searchMatch.insert({node->getId(), true});
            } else {
                searchMatch.erase(node->getId());
            }
            searchMatchWith.insert({node->getId(), search});
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
            renderComponents(node);
            for (const auto child: node->children) {
                renderNode(child);
            }
        }

        ImGui::TreePop();
    }

    void HierarchyPanel::renderComponents(const Entity *node) const {
        if (!editorRepository->showOnlyEntitiesHierarchy) {
            for (auto &val: node->components) {
                addComponent(val.second);
            }
        }
    }

    void HierarchyPanel::addComponent(AbstractComponent *component) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("%s%s", component->getIcon(), component->getTitle());
        ImGui::TableNextColumn();
        ImGui::TextDisabled("--");
        ImGui::TableNextColumn();
        ImGui::TextDisabled("--");
    }

    int HierarchyPanel::getFlags(const Entity *node) {
        int flags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (isOnSearch) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
        return flags | (opened.contains(node->getId()) ? opened[node->getId()] : ImGuiTreeNodeFlags_None);
    }

    void HierarchyPanel::renderEntityColumns(const Entity *node, const bool isPinned) {
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
            changeVisibilityRecursively(node->getId(), !isVisible);
        }
        ImGui::TableNextColumn();
        bool isNodePinned = editorRepository->pinnedEntities.contains(node->getId());
        if (UIUtil::ButtonSimple(
            ((isNodePinned ? Icons::lock : Icons::lock_open) + (isPinned ? "##ppinned" : "##p") + idString) + id,
            20, 15)) {
            if (isNodePinned) {
                editorRepository->pinnedEntities.erase(node->getId());
            } else {
                editorRepository->pinnedEntities.insert({node->getId(), true});
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    void HierarchyPanel::changeVisibilityRecursively(EntityID node, const bool isVisible) {
        if (isVisible) {
            world->hiddenEntities.erase(node);
        } else {
            world->hiddenEntities.insert({node, true});
        }

        for (const auto child: world->getEntity(node)->children) {
            changeVisibilityRecursively(child, isVisible);
        }
    }

    void HierarchyPanel::handleClick(const Entity *node) const {
        if (ImGui::IsItemClicked()) {
            if (const bool isMultiSelect = ImGui::IsKeyDown(ImGuiKey_LeftCtrl); !isMultiSelect) {
                context->getEditorContext().selectionService.clearSelection();
            }
            context->getEditorContext().selectionService.addSelected(node->getId());
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
