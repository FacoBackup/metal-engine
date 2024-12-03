#include "InspectorPanel.h"

namespace Metal {
    void InspectorPanel::onInitialize() {
        cameraRepository = &context->getEngine().getCameraSystem().cameraRepository;
    }

    void InspectorPanel::onSync() {
        cameraRepository->getFields();
    }
}
