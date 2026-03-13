#include "RepositoriesPanel.h"
#include "../../abstract/form/FormPanel.h"
#include "../../../ApplicationContext.h"
#include "../../../common/Inspectable.h"

namespace Metal {
    void RepositoriesPanel::onInitialize() {
        formPanel = new FormPanel();
        appendChild(formPanel);
        
        repositories.push_back(&applicationContext->editorRepository);
        repositories.push_back(&applicationContext->engineRepository);
        repositories.push_back(&applicationContext->worldRepository.camera);
    }

    void RepositoriesPanel::onSync() {
        for (auto *repo : repositories) {
            formPanel->setInspection(repo);
        }
        formPanel->onSync();
    }
}
