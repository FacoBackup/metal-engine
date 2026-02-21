#include "InspectorPanel.h"

#include "../../abstract/form/FormPanel.h"
#include "../../../../util/UIUtil.h"
#include "../../../../common/inspection/Inspectable.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/impl/EntityComponent.h"
#include "../../../../repository/world/components/VolumeComponent.h"
#include "../../../../repository/world/components/LightComponent.h"
#include "../../../../service/camera/Camera.h"

namespace Metal {

    void InspectorPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
        Inspectable* editorRepo = &CTX.editorRepository;
        Inspectable* engineRepo = &CTX.engineRepository;
        Inspectable* cameraRepo = &CTX.worldRepository.camera;
        repositories.push_back(editorRepo);
        repositories.push_back(engineRepo);
        repositories.push_back(cameraRepo);
    }

    void InspectorPanel::onSync() {
        const auto &editorRepository = CTX.editorRepository;
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
        }
        ImGui::EndChild();
        ImGui::Columns(1);
    }

    void InspectorPanel::tick() {
        if (auto &editorRepository = CTX.editorRepository;
            editorRepository.mainSelection != selectedId) {
            currentInspection = nullptr;
            additionalInspection.clear();
            selectedId = editorRepository.mainSelection;

            if (selectedId != EMPTY_ENTITY) {
                auto &repo = CTX.worldRepository;
                const auto entity = static_cast<entt::entity>(selectedId);

                selectedEntity = repo.getEntity(selectedId);
                if (selectedEntity != nullptr) {
                    additionalInspection.push_back(selectedEntity);

                    if (repo.registry.all_of<MeshComponent>(entity)) {
                        additionalInspection.push_back((Inspectable*)&repo.registry.get<MeshComponent>(entity));
                    }
                    if (repo.registry.all_of<TransformComponent>(entity)) {
                        additionalInspection.push_back((Inspectable*)&repo.registry.get<TransformComponent>(entity));
                    }
                    if (repo.registry.all_of<std::unique_ptr<LightComponent>>(entity)) {
                        additionalInspection.push_back((Inspectable*)repo.registry.get<std::unique_ptr<LightComponent>>(entity).get());
                    }
                    if (repo.registry.all_of<VolumeComponent>(entity)) {
                        additionalInspection.push_back((Inspectable*)&repo.registry.get<VolumeComponent>(entity));
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
