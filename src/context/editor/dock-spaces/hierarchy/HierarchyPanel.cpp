#include "HierarchyPanel.h"

#include "../../../../common/interface/Icons.h"
#include "../../../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/impl/Entity.h"
#include "../../../../enum/ComponentType.h"

#define BUTTON_HEIGHT 22
#define BUTTON_PADDING 24.0f

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

    void HierarchyPanel::renderPinButton(EntityID entityID, bool isSelected) {
        ImGui::SameLine();
        ImGui::PushID((entityID + "pinButton" + id).c_str());
        if (isSelected) ImGui::PushStyleColor(ImGuiCol_Button, context->editorRepository.accent);
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.f, .5f));
        if (ImGui::Button(!isSelected ? Icons::lock_open.c_str() : Icons::lock.c_str(),
                          ImVec2(BUTTON_HEIGHT, BUTTON_HEIGHT))) {
            if (!isSelected) {
                context->selectionService.addSelected(entityID); // Add only this entity
            } else {
                context->selectionService.removeSelected(entityID);
            }
        }
        if (isSelected) ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void HierarchyPanel::renderHideButton(EntityID entityID, bool isHidden) {
        ImGui::SameLine();
        ImGui::PushID((entityID + "pinButton" + id).c_str());
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.f, .5f));
        if (ImGui::Button(!isHidden ? Icons::visibility.c_str() : Icons::visibility_off.c_str(),
                          ImVec2(BUTTON_HEIGHT, BUTTON_HEIGHT))) {
            context->worldRepository.changeVisibility(entityID, !isHidden);
        }
        ImGui::PopStyleVar();

        ImGui::PopID();
    }

    void HierarchyPanel::DrawEntityNode(EntityID entityID, int depth) {
        Entity *entity = context->worldRepository.getEntity(entityID);
        if (!entity || !MatchesSearchRecursive(entity)) return;

        bool isHidden = context->worldRepository.hiddenEntities.contains(entityID) &&
                        context->worldRepository.hiddenEntities[entityID];
        bool isSelected = context->editorRepository.selected.contains(entityID) &&
                          context->editorRepository.selected[entityID];

        ImVec2 buttonSize(ImGui::GetContentRegionAvail().x - (BUTTON_HEIGHT + 8) * 2, BUTTON_HEIGHT);
        ImGui::PushID(entityID);
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_Text,
                              ImVec4(entity->color.x, entity->color.y, entity->color.z, isHidden ? 0.5f : 1.0f));

        const char *arrowIcon = entity->children.empty()
                                    ? ""
                                    : (entity->isOpen ? Icons::arrow_drop_down.c_str() : Icons::arrow_right.c_str());
        const char *entityIcon = GetIcon(entity);

        float padding = (BUTTON_PADDING * depth) / buttonSize.x;
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(padding, 0.5f));

        if (ImGui::Button((std::string(arrowIcon) + " " + entityIcon + " " + entity->name).c_str(), buttonSize)) {
            entity->isOpen = !entity->isOpen;
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        handleDrag(entityID, entity);
        handleDrop(entityID, entity);
        renderPinButton(entityID, isSelected);
        renderHideButton(entityID, isHidden);

        if (drawContextMenu(entityID, entity, isHidden)) return;

        if (entity->isOpen) {
            for (EntityID childID: entity->children) {
                DrawEntityNode(childID, depth + 1);
            }
        }

        ImGui::PopID();
    }

    void HierarchyPanel::onSync() {
        ImGui::InputText((id + "Search").c_str(), searchBuffer, IM_ARRAYSIZE(searchBuffer));
        ImGui::SameLine();
        if (ImGui::Button((Icons::inventory_2 + id).c_str())) {
            context->worldRepository.createEntity("New Folder", true);
        }

        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
        if (context->worldRepository.entities.count(WorldRepository::ROOT_ID)) {
            DrawEntityNode(WorldRepository::ROOT_ID);
        }
        ImGui::PopStyleVar();

    }
} // Metal
