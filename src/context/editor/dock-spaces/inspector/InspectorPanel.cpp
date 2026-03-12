#include "InspectorPanel.h"

#include "../../abstract/form/FormPanel.h"
#include "../../../../util/UIUtil.h"
#include "../../../../common/inspection/Inspectable.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/impl/MetadataComponent.h"
#include <imgui.h>
#include <format>

namespace Metal {
    void InspectorPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
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
                    bool hasComponent = compDef.getInspectable(CTX.worldRepository, selectedId) != nullptr;
                    if (!hasComponent) {
                        if (ImGui::MenuItem(
                            (compDef.icon + " " + compDef.name + id + "adCOmp" + compDef.name).c_str())) {
                            CTX.worldRepository.createComponent(selectedId, compDef.type);
                            selectedId = EMPTY_ENTITY;
                            tick();
                        }
                    }
                }
                ImGui::EndPopup();
            }
        }
    }

    void InspectorPanel::tick() {
        if (auto &editorRepository = CTX.editorRepository;
            editorRepository.mainSelection != selectedId) {
            additionalInspection.clear();
            selectedId = editorRepository.mainSelection;
            formPanel->resetForm();
            if (selectedId != EMPTY_ENTITY) {
                auto &repo = CTX.worldRepository;

                selectedEntity = repo.getEntity(selectedId);
                if (selectedEntity != nullptr) {
                    additionalInspection.push_back(selectedEntity);

                    for (const auto &compDef: ComponentTypes::getComponents()) {
                        if (Inspectable *inspectable = compDef.getInspectable(repo, selectedId)) {
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
