#include "WorldPanel.h"
#include "WorldHeaderPanel.h"
#include "../../../common/Icons.h"
#include "../../util/UIUtil.h"
#include "../../../ApplicationContext.h"
#include "../../../engine/dto/MetadataComponent.h"
#include "../../../engine/enum/ComponentType.h"
#include "../../../engine/repository/WorldRepository.h"
#include "../../repository/EditorRepository.h"
#include "../../service/SelectionService.h"
#include "../../../core/DirectoryService.h"

namespace Metal {
    void WorldPanel::onInitialize() {
        headerPanel = initializePanel<WorldHeaderPanel>();

        shortcuts = {
            ShortcutDTO("Delete", ImGuiKey_Delete, [this]() {
                std::vector<entt::entity> entities;
                for (auto &entry: editorRepository->selected) {
                    entities.push_back(entry.first);
                }
                world->deleteEntities(entities);
                selectionService->clearSelection();
            }),
            ShortcutDTO("Select All", ImGuiMod_Ctrl | ImGuiKey_A, [this]() {
                std::vector<entt::entity> entities;
                for (auto entity : world->registry.view<entt::entity>()) {
                    if (isMatched(entity)) {
                        entities.push_back(entity);
                    }
                }
                selectionService->addAllSelected(entities);
            })
        };
    }

    void WorldPanel::contextMenu() const {
        if (ImGui::BeginPopupContextItem((id + "contextMenu").c_str())) {
            if (ImGui::MenuItem("Delete")) {
                std::vector<entt::entity> entities;
                for (auto &entry: editorRepository->selected) {
                    entities.push_back(entry.first);
                }
                world->deleteEntities(entities);
                selectionService->clearSelection();
            }
            ImGui::EndPopup();
        }

        if (isSomethingHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup((id + "contextMenu").c_str());
        }
    }

    void WorldPanel::onSync() {
        isOnSearch = strlen(headerPanel->search) > 0 || headerPanel->selectedComponentType != -1;
        isSomethingHovered = ImGui::IsItemHovered();
        onSyncChildren();
        ImGui::Separator();

        bool isEmpty = true;
        auto view = world->registry.view<MetadataComponent>();
        for (const auto entity: view) {
            if (!isOnSearch || isMatched(entity)) {
                isEmpty = false;
                break;
            }
        }

        if (isEmpty) {
            const char *text = isOnSearch ? "No entities match search" : "No entities in world";
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 textSize = ImGui::CalcTextSize(text);

            ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f));
            ImGui::TextDisabled("%s", text);
        } else {
            if (ImGui::BeginTable((id + "hierarchyTable").c_str(), 2, TABLE_FLAGS)) {
                isSomethingHovered = isSomethingHovered || ImGui::IsItemHovered();
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn(Icons::visibility.c_str(), ImGuiTableColumnFlags_WidthFixed, 20.f);
                ImGui::TableHeadersRow();

                for (const auto entity: view) {
                    renderNode(entity);
                }

                ImGui::EndTable();
            }
        }

        if (!editorRepository->selected.empty()) {
            contextMenu();
        }
    }

    void WorldPanel::renderNode(const entt::entity entityId) {
        MetadataComponent *node = world->getEntity(entityId);
        if (node == nullptr || (isOnSearch && !isMatched(entityId))) {
            return;
        }

        applyRowStyle(entityId, node);

        if (processEntityNode(entityId, node)) {
            renderEntityChildren(entityId);
            ImGui::TreePop();
        }
    }

    bool WorldPanel::processEntityNode(const entt::entity entityId, MetadataComponent *node) {
        ImGui::TableNextRow();
        if (editorRepository->selected.contains(entityId)) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, editorRepository->accentU32);
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, editorRepository->accentU32);
        }

        ImGui::TableNextColumn();
        const bool open = ImGui::TreeNodeEx(getNodeLabel(entityId).c_str(), getFlags(entityId));

        handleDragDrop(entityId);
        renderEntityColumns(entityId);
        return open;
    }

    void WorldPanel::applyRowStyle(const entt::entity entityId, MetadataComponent *node) const {
        ImVec4 color = ImVec4(node->color.x, node->color.y, node->color.z, 1.0f);
        if (world->culled.contains(entityId) || world->hiddenEntities.contains(entityId)) {
            color.w = 0.5f;
        }
        ImGui::PushStyleColor(ImGuiCol_Text, color);
    }

    std::string WorldPanel::getNodeLabel(const entt::entity entityId) const {
        const auto node = world->getEntity(entityId);
        return std::format("{} {}##{}{}", Icons::category, node->name, entt::to_integral(entityId), id);
    }

    bool WorldPanel::isMatched(const entt::entity entityId) const {
        const auto node = world->getEntity(entityId);
        if (node == nullptr) return false;
        bool matches = node->name.find(headerPanel->search) != std::string::npos;
        if (headerPanel->selectedComponentType != -1) {
            bool hasComponent = false;
            for (const auto &compDef : ComponentTypes::getComponents()) {
                if (static_cast<int>(compDef.type) == headerPanel->selectedComponentType) {
                    hasComponent = compDef.getInspectable(*world, entityId) != nullptr;
                    break;
                }
            }
            matches &= hasComponent;
        }
        return matches;
    }

    void WorldPanel::renderEntityChildren(const entt::entity entityId) {
        for (const auto &compDef: ComponentTypes::getComponents()) {
            if (compDef.getInspectable(*world, entityId)) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                                     ImGuiTreeNodeFlags_SpanFullWidth;
                ImGui::TreeNodeEx((compDef.icon + " " + compDef.name + "##comp" + std::to_string(static_cast<uint32_t>(entt::to_integral(entityId)))).c_str(),
                                  flags);

                ImGui::TableNextColumn(); // Visibility column
            }
        }
    }

    int WorldPanel::getFlags(const entt::entity entityId) const {
        int flags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (isOnSearch) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
        return flags;
    }

    void WorldPanel::renderEntityColumns(const entt::entity entityId) const {
        handleClick(entityId);
        ImGui::TableNextColumn();

        ImGui::PushStyleColor(ImGuiCol_Button, TRANSPARENT);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, PADDING);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

        const bool isVisible = !world->hiddenEntities.contains(entityId);
        if (UIUtil::ButtonSimple((isVisible ? Icons::visibility : Icons::visibility_off) + ("##v") +
                                 std::to_string(static_cast<uint32_t>(entt::to_integral(entityId))) + id, 20, 15)) {
            world->changeVisibility(entityId, !isVisible);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(); // Pop Row Style (from applyRowStyle)
    }

    void WorldPanel::handleClick(const entt::entity entityId) const {
        if (ImGui::IsItemClicked()) {
            if (const bool isMultiSelect = ImGui::IsKeyDown(ImGuiKey_LeftCtrl); !isMultiSelect) {
                selectionService->clearSelection();
            }
            selectionService->addSelected(entityId);
        }
    }

    void WorldPanel::handleDragDrop(const entt::entity entityId) {
        const auto node = world->getEntity(entityId);
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(id.c_str(), id.c_str(), sizeof(id.c_str()));
            onDrag = entityId;
            ImGui::Text("Dragging Node %s", node->getTitle());
            ImGui::EndDragDropSource();
        }
    }
} // Metal
