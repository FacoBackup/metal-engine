#include "HierarchyPanel.h"

#include "HierarchyHeaderPanel.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/impl/Entity.h"
#include "../../../../enum/ComponentType.h"

namespace Metal {
    const char *HierarchyPanel::GetIcon(const Entity *node) {
        static constexpr std::array<ComponentTypes::ComponentType, 3> priorityComponents = {
            ComponentTypes::MESH, ComponentTypes::LIGHT, ComponentTypes::VOLUME
        };

        for (const auto comp: node->components) {
            for (const auto type: priorityComponents) {
                if (comp == type) {
                    return ComponentTypes::IconOf(type);
                }
            }
        }
        return Icons::inventory_2.c_str();
    }

    bool HierarchyPanel::MatchesSearchRecursive(Entity *entity) {
        if (searchBuffer[0] == '\0' || entity->name.find(searchBuffer) != std::string::npos) {
            return true;
        }

        for (EntityID childID: entity->children) {
            Entity *child = context->worldRepository.getEntity(childID);
            if (child && MatchesSearchRecursive(child)) {
                return true;
            }
        }

        return false;
    }

    void HierarchyPanel::handleDrop(EntityID entityID, Entity *entity) {
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG")) {
                EntityID droppedEntityID = *(const EntityID *) payload->Data;
                if (droppedEntityID != entityID) {
                    Entity *droppedEntity = context->worldRepository.getEntity(droppedEntityID);
                    if (droppedEntity && droppedEntity->parent != entityID) {
                        // Prevent cycles
                        context->worldRepository.linkEntities(entity, droppedEntity);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    void HierarchyPanel::handleDrag(EntityID entityID, Entity *entity) {
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("ENTITY_DRAG", &entityID, sizeof(EntityID));
            ImGui::Text("Dragging %s", entity->name.c_str());
            ImGui::EndDragDropSource();
        }
    }

    bool HierarchyPanel::drawContextMenu(EntityID entityID, Entity *entity, bool isHidden) {
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Create Folder")) {
                EntityID newFolder = context->worldRepository.createEntity("New Folder", true);
                context->worldRepository.linkEntities(entity, context->worldRepository.getEntity(newFolder));
            }
            if (ImGui::MenuItem("Delete")) {
                context->worldRepository.deleteEntities({entityID});
                ImGui::EndPopup();
                return true;
            }
            if (ImGui::MenuItem(isHidden ? "Show" : "Hide")) {
                context->worldRepository.changeVisibility(entityID, !isHidden);
            }
            ImGui::EndPopup();
        }
        return false;
    }

    void HierarchyPanel::handleSelection(EntityID entityID) {
        if (ImGui::IsItemClicked()) {
            if (ImGui::GetIO().KeyCtrl) {
                context->selectionService.addSelected(entityID); // CTRL + click: add to selection
            } else {
                context->selectionService.clearSelection(); // Click: clear previous selection
                context->selectionService.addSelected(entityID); // Add only this entity
            }
        }
    }

    void HierarchyPanel::renderHideButton(EntityID entityID, bool isHidden) {
        ImGui::SameLine();
        ImGui::PushID(entityID); // Ensure unique ID for the button
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        if (ImGui::Button(!isHidden ? Icons::visibility.c_str() : Icons::visibility_off.c_str(), ImVec2(25,   16))) {
            context->worldRepository.changeVisibility(entityID, !isHidden);
        }
        ImGui::PopStyleVar();

        ImGui::PopID();
    }

    void HierarchyPanel::DrawEntityNode(EntityID entityID) {
        Entity *entity = context->worldRepository.getEntity(entityID);
        if (!entity || !MatchesSearchRecursive(entity)) return;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (entity->children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

        bool isHidden = context->worldRepository.hiddenEntities.contains(entityID) &&
                        context->worldRepository.hiddenEntities[entityID];

        bool isSelected = context->editorRepository.selected.contains(entityID) &&
                          context->editorRepository.selected[entityID];

        if (isHidden) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        if (isSelected) ImGui::PushStyleColor(ImGuiCol_Header, context->editorRepository.accent);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));

        const char *icon = GetIcon(entity);
        bool nodeOpen = ImGui::TreeNodeEx((void *) (intptr_t) entityID,
                                          flags | (isSelected ? ImGuiTreeNodeFlags_Selected : 0),
                                          "%s %s", icon, entity->name.c_str());

        ImGui::PopStyleVar();
        if (isHidden) ImGui::PopStyleColor();
        if (isSelected) ImGui::PopStyleColor();

        // Handle selection
        handleSelection(entityID);

        // Context Menu
        if (drawContextMenu(entityID, entity, isHidden)) return;

        // Drag & Drop Source
        handleDrag(entityID, entity);

        // Drag & Drop Target
        handleDrop(entityID, entity);

        // Add margin before the button
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

        renderHideButton(entityID, isHidden);

        // Recursively Draw Children
        if (nodeOpen) {
            for (EntityID childID: entity->children) {
                DrawEntityNode(childID);
            }
            ImGui::TreePop();
        }
    }

    void HierarchyPanel::onSync() {
        ImGui::InputText("##Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));
        ImGui::SameLine();
        if (ImGui::Button(("+" + Icons::inventory_2).c_str())) {
            context->worldRepository.createEntity("New Folder", true);
        }

        ImGui::Separator();

        if (context->worldRepository.entities.count(WorldRepository::ROOT_ID)) {
            DrawEntityNode(WorldRepository::ROOT_ID);
        }
    }
} // Metal
