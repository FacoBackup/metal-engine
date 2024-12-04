#include "InspectorPanel.h"

namespace Metal {
    void InspectorPanel::onInitialize() {
        cameraRepository = &context->getEngine().cameraRepository;
    }

    void InspectorPanel::onSync() {
        cameraRepository->getFields();
    }
}
