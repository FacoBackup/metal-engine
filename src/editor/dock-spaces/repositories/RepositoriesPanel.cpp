#include "RepositoriesPanel.h"
#include "../../abstract/form/FormPanel.h"
#include "../../../ApplicationContext.h"
#include "../../../common/Inspectable.h"
#include "../../repository/EditorRepository.h"
#include "../../../engine/repository/EngineRepository.h"
#include "../../../engine/repository/WorldRepository.h"
#include "../../../engine/dto/Camera.h"

namespace Metal {
    void RepositoriesPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
        
        repositories.push_back(editorRepository);
        repositories.push_back(engineRepository);
        repositories.push_back(&worldRepository->camera);
    }

    void RepositoriesPanel::onSync() {
        for (auto *repo : repositories) {
            formPanel->setInspection(repo);
        }
        formPanel->onSync();
    }
}
