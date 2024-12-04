#include "InspectorPanel.h"
#include "../../common/form/FormPanel.h"

namespace Metal {
    void InspectorPanel::onInitialize() {
        cameraRepository = &context->getEngine().cameraRepository;
        auto *form = new FormPanel();
        appendChild(form);
        form->setInspection(cameraRepository);
    }

    void InspectorPanel::onSync() {
        onSyncChildren();
    }
}
