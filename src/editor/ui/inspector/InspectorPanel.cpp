#include "InspectorPanel.h"
#include "../../common/form/FormPanel.h"
#include "../../../common/util/UIUtil.h"
#include "../../../common/inspection/Inspectable.h"
#include "../../../context/ApplicationContext.h"
#include "../../../engine/service/world/impl/Entity.h"
#include "../../../engine/service/world/impl/AbstractComponent.h"
#include "../../../engine/service/camera/Camera.h"

namespace Metal {
    void InspectorPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
        repositories.push_back(&context->getEditorContext().editorRepository);
        repositories.push_back(&context->engineContext.atmosphereRepository);
        repositories.push_back(&context->getEngineContext().worldRepository.camera);
    }

    void InspectorPanel::onSync() {
        const auto &editorRepository = context->getEditorContext().editorRepository;
        tick();
        ImGui::Columns(2, (id + "inspectorColumns").c_str(), false);
        ImGui::SetColumnWidth(0, 30);
        for (auto *repo: repositories) {
            if (UIUtil::RenderOption(repo->getIcon(), currentInspection == repo, true,
                                     editorRepository.accent)) {
                currentInspection = repo;
            }
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
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::NextColumn();
        if (ImGui::BeginChild((id + "form").c_str())) {
            formPanel->setInspection(currentInspection);
            formPanel->onSync();
        }
        ImGui::EndChild();
        ImGui::Columns(1);
    }

    void InspectorPanel::tick() {
        if (auto &editorRepository = context->getEditorContext().editorRepository;
            editorRepository.mainSelection != selectedId) {
            additionalInspection.clear();
            selectedId = editorRepository.mainSelection;

            if (selectedId != EMPTY_ENTITY) {
                selectedEntity = context->getEngineContext().worldRepository.getEntity(selectedId);
                if (selectedEntity != nullptr) {
                    additionalInspection.push_back(selectedEntity);
                    for (auto &comp: context->getEngineContext().worldRepository.getEntity(selectedId)->components) {
                        additionalInspection.push_back(comp.second);
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
