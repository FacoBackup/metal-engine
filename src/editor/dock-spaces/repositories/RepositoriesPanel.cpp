#include "RepositoriesPanel.h"
#include "../../abstract/form/FormPanel.h"
#include "../../../ApplicationContext.h"
#include "../../../common/Inspectable.h"

namespace Metal {
    void RepositoriesPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
        
        repositories.push_back(&CTX.editorRepository);
        repositories.push_back(&CTX.engineRepository);
        repositories.push_back(&CTX.worldRepository.camera);
    }

    void RepositoriesPanel::onSync() {
        for (auto *repo : repositories) {
            formPanel->setInspection(repo);
        }
        formPanel->onSync();
    }
}
