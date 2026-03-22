#include "InspectorPanel.h"
#include "../../abstract/form/FormPanel.h"
#include "../../util/UIUtil.h"
#include "../../../common/Inspectable.h"
#include "../../../ApplicationContext.h"
#include "../../../engine/dto/MetadataComponent.h"
#include <imgui.h>
#include <format>
#include "../../repository/EditorRepository.h"
#include "../../../engine/repository/WorldRepository.h"

namespace Metal {
    void InspectorPanel::onInitialize() {
        formPanel = initializePanel<FormPanel>();
    }

    void InspectorPanel::onSync() {
        tick();
        formPanel->onSync();

        if (selectedId != EMPTY_ENTITY) {
            ImGui::Separator();
            if (ImGui::Button((Icons::add + " Add Component" + id + "addComp").c_str(), ImVec2(-1, 0))) {
                ImGui::OpenPopup((id + "AddComponentPopup").c_str());
            }

            if (ImGui::BeginPopup((id + "AddComponentPopup").c_str())) {
                for (const auto &compDef: ComponentTypes::getComponents()) {
                    bool hasComponent = compDef.getInspectable(*worldRepository, selectedId) != nullptr;
                    if (!hasComponent) {
                        if (ImGui::MenuItem(
                            (compDef.icon + " " + compDef.name + id + "adCOmp" + compDef.name).c_str())) {
                            worldRepository->createComponent(selectedId, compDef.type);
                            selectedId = EMPTY_ENTITY;
                            tick();
                        }
                    }
                }
                ImGui::EndPopup();
            }
        } else {
            const char *text = "Select an entity to inspect";
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 textSize = ImGui::CalcTextSize(text);

            ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f));
            ImGui::TextDisabled("%s", text);
        }
    }

    void InspectorPanel::tick() {
        if (editorRepository->mainSelection != selectedId) {
            additionalInspection.clear();
            selectedId = editorRepository->mainSelection;
            formPanel->resetForm();
            if (selectedId != EMPTY_ENTITY) {
                selectedEntity = worldRepository->getEntity(selectedId);
                if (selectedEntity != nullptr) {
                    additionalInspection.push_back(selectedEntity);

                    for (const auto &compDef: ComponentTypes::getComponents()) {
                        if (Inspectable *inspectable = compDef.getInspectable(*worldRepository, selectedId)) {
                            additionalInspection.push_back(inspectable);
                        }
                    }
                }
            } else {
                selectedEntity = nullptr;
            }
        }

        for (auto *component: additionalInspection) {
            formPanel->setInspection(component);
        }
    }
}
