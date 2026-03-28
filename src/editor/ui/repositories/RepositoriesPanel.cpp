#include "RepositoriesPanel.h"
#include "editor/ui/abstract/form/FormPanel.h"
#include "ApplicationContext.h"
#include "common/Reflection.h"
#include "common/IRepository.h"
#include "engine/repository/WorldRepository.h"
#include "engine/dto/Camera.h"

namespace Metal {
    void RepositoriesPanel::onInitialize() {
        formPanel = initializePanel<FormPanel>();
        
        auto repos = ctx->getSingletons<IRepository>();
        for (auto* repo : repos) {
            if (!repo->getFields().empty()) {
                repositories.push_back(repo);
            }
        }
    }

    void RepositoriesPanel::onSync() {
        for (auto *repo : repositories) {
            formPanel->setInspection(repo);
        }
        formPanel->onSync();
    }
}
