#include "InspectorPanel.h"

#include "MaterialInspection.h"
#include "../../abstract/form/FormPanel.h"
#include "../../../util/UIUtil.h"
#include "../../../../common/inspection/Inspectable.h"
#include "../../../../MetalContextProvider.h"
#include "../../../../runtime/data/Entity.h"
#include "../../../../runtime/data/components/AbstractComponent.h"
#include "../../../../runtime/data/Camera.h"

namespace Metal {
    void InspectorPanel::onInitialize() {
        formPanel = new FormPanel();
        materialPanel = new MaterialInspection();
        appendChild(formPanel);
        appendChild(materialPanel);
        repositories.push_back(&SINGLETONS.editorRepository);
        repositories.push_back(&SINGLETONS.engineRepository);
        repositories.push_back(&SINGLETONS.worldRepository.camera);
        repositories.push_back(&SINGLETONS.fileInspection);
    }

    void InspectorPanel::onSync() {
        const auto &editorRepository = SINGLETONS.editorRepository;
        tick();
        ImGui::Columns(2, (id + "inspectorColumns").c_str(), false);
        ImGui::SetColumnWidth(0, 30);
        for (auto *repo: repositories) {
            if (UIUtil::RenderOption(repo->getIcon(), currentInspection == repo, true,
                                     editorRepository.accent)) {
                currentInspection = repo;
            }
            UIUtil::RenderTooltip(repo->getTitle());
        }

        ImGui::Spacing();
        ImGui::Spacing();
        for (auto *additional: additionalInspection) {
            if (additional != nullptr) {
                if (UIUtil::RenderOption(
                    std::format("{}##{}", additional->getIcon(), additional->getUniqueId()),
                    currentInspection == additional,
                    true,
                    editorRepository.accent)) {
                    currentInspection = additional;
                }
                UIUtil::RenderTooltip(additional->getTitle());
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::NextColumn();
        if (ImGui::BeginChild((id + "form").c_str())) {
            formPanel->setInspection(currentInspection);
            formPanel->onSync();

            if (currentInspection == &SINGLETONS.fileInspection) {
                materialPanel->onSync();
            }
        }
        ImGui::EndChild();
        ImGui::Columns(1);
    }

    void InspectorPanel::tick() {
        if (auto &editorRepository = SINGLETONS.editorRepository;
            editorRepository.mainSelection != selectedId) {
            currentInspection = nullptr;
            additionalInspection.clear();
            selectedId = editorRepository.mainSelection;

            if (selectedId != EMPTY_ENTITY) {
                selectedEntity = SINGLETONS.worldRepository.getEntity(selectedId);
                if (selectedEntity != nullptr) {
                    additionalInspection.push_back(selectedEntity);
                    for (const auto &comp: SINGLETONS.worldRepository.getEntity(selectedId)->components) {
                        additionalInspection.push_back(SINGLETONS.worldRepository.getComponent(comp, selectedId));
                    }
                    currentInspection = additionalInspection[0];
                }
            } else {
                selectedEntity = nullptr;
            }

            if (selectedId != EMPTY_ENTITY && selectedEntity == nullptr) {
                currentInspection = repositories[0];
            }
        }
    }
}
