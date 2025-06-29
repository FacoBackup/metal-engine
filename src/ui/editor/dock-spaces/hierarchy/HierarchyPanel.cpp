#include "HierarchyPanel.h"

#include "../../../../common/enum/Icons.h"
#include "../../../util/UIUtil.h"
#include "../../../../MetalContextProvider.h"
#include "../../../../runtime/data/Entity.h"
#include "../../../../common/enum/ComponentType.h"

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
            Entity *child = SINGLETONS.worldRepository.getEntity(childID);
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
                    Entity *droppedEntity = SINGLETONS.worldRepository.getEntity(droppedEntityID);
                    if (droppedEntity && droppedEntity->parent != entityID) {
                        // Prevent cycles
                        SINGLETONS.worldRepository.linkEntities(entity, droppedEntity);
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
                EntityID newFolder = SINGLETONS.worldRepository.createEntity("New Folder", true);
                SINGLETONS.worldRepository.linkEntities(entity, SINGLETONS.worldRepository.getEntity(newFolder));
            }
            if (ImGui::MenuItem("Delete")) {
                SINGLETONS.worldRepository.deleteEntities({entityID});
                ImGui::EndPopup();
                return true;
            }
            if (ImGui::MenuItem(isHidden ? "Show" : "Hide")) {
                SINGLETONS.worldRepository.changeVisibility(entityID, !isHidden);
            }
            ImGui::EndPopup();
        }
        return false;
    }

    void HierarchyPanel::handleSelection(EntityID entityID) {
        if (ImGui::IsItemClicked()) {
            if (ImGui::GetIO().KeyCtrl) {
                SINGLETONS.selectionService.addSelected(entityID); // CTRL + click: add to selection
            } else {
                SINGLETONS.selectionService.clearSelection(); // Click: clear previous selection
                SINGLETONS.selectionService.addSelected(entityID); // Add only this entity
            }
        }
    }

    void HierarchyPanel::renderHideButton(EntityID entityID, bool isHidden) {
        ImGui::SameLine();
        ImGui::PushID(entityID); // Ensure unique ID for the button
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 0));
        if (ImGui::Button(!isHidden ? Icons::visibility.c_str() : Icons::visibility_off.c_str(), ImVec2(24, 16))) {
            SINGLETONS.worldRepository.changeVisibility(entityID, !isHidden);
        }
        ImGui::PopStyleVar();

        ImGui::PopID();
    }

    void HierarchyPanel::DrawEntityNode(EntityID entityID) {
        Entity *entity = SINGLETONS.worldRepository.getEntity(entityID);
        if (!entity || !MatchesSearchRecursive(entity)) return;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (entity->children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

        bool isHidden = SINGLETONS.worldRepository.hiddenEntities.contains(entityID) &&
                        SINGLETONS.worldRepository.hiddenEntities[entityID];

        bool isSelected = SINGLETONS.editorRepository.selected.contains(entityID) &&
                          SINGLETONS.editorRepository.selected[entityID];

        if (isSelected) ImGui::PushStyleColor(ImGuiCol_Header, SINGLETONS.editorRepository.accent);
        ImGui::PushStyleColor(ImGuiCol_Text,
                              ImVec4(entity->color.x, entity->color.y, entity->color.z, isHidden ? .5 : 1.0f));

        const char *icon = GetIcon(entity);
        bool nodeOpen = ImGui::TreeNodeEx((void *) (intptr_t) entityID,
                                          flags | (isSelected ? ImGuiTreeNodeFlags_Selected : 0),
                                          "%s %s", icon, entity->name.c_str());

        ImGui::PopStyleColor();
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
            SINGLETONS.worldRepository.createEntity("New Folder", true);
        }

        ImGui::Separator();

        if (SINGLETONS.worldRepository.entities.count(WorldRepository::ROOT_ID)) {
            DrawEntityNode(WorldRepository::ROOT_ID);
        }
    }
} // Metal
