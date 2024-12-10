#include "InspectorPanel.h"
#include "../../common/form/FormPanel.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void InspectorPanel::onInitialize() {

        cameraRepository = &context->getEngineContext().cameraRepository;
        auto *form = new FormPanel();
        appendChild(form);
        form->setInspection(cameraRepository);
    }

    void InspectorPanel::onSync() {
        onSyncChildren();
    }
}
