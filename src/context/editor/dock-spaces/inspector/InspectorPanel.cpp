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
        Inspectable *editorRepo = &CTX.editorRepository;
        Inspectable *engineRepo = &CTX.engineRepository;
        Inspectable *cameraRepo = &CTX.worldRepository.camera;
        repositories.push_back(editorRepo);
        repositories.push_back(engineRepo);
        repositories.push_back(cameraRepo);
    }

    void InspectorPanel::onSync() {
        tick();
        {
            for (auto *repo: repositories) {
                formPanel->setInspection(repo);
            }
            for (auto *component: additionalInspection) {
                formPanel->setInspection(component);
            }
        }
        formPanel->onSync();
    }

    void InspectorPanel::tick() {
        if (auto &editorRepository = CTX.editorRepository;
            editorRepository.mainSelection != selectedId) {
            additionalInspection.clear();
            selectedId = editorRepository.mainSelection;
            formPanel->resetForm();
            if (selectedId != EMPTY_ENTITY) {
                auto &repo = CTX.worldRepository;
                const auto entity = static_cast<entt::entity>(selectedId);

                selectedEntity = repo.getEntity(selectedId);
                if (selectedEntity != nullptr) {
                    additionalInspection.push_back(selectedEntity);

                    if (repo.registry.all_of<MeshComponent>(entity)) {
                        additionalInspection.push_back((Inspectable *) &repo.registry.get<MeshComponent>(entity));
                    }
                    if (repo.registry.all_of<TransformComponent>(entity)) {
                        additionalInspection.push_back((Inspectable *) &repo.registry.get<TransformComponent>(entity));
                    }
                    if (repo.registry.all_of<std::unique_ptr<LightComponent> >(entity)) {
                        additionalInspection.push_back(
                            (Inspectable *) repo.registry.get<std::unique_ptr<LightComponent> >(entity).get());
                    }
                    if (repo.registry.all_of<VolumeComponent>(entity)) {
                        additionalInspection.push_back((Inspectable *) &repo.registry.get<VolumeComponent>(entity));
                    }
                }
            } else {
                selectedEntity = nullptr;
            }
        }
    }
}
