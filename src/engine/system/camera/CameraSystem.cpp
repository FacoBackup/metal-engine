#include "CameraSystem.h"
#include "../../../common/runtime/ApplicationContext.h"

namespace Metal {
    void CameraSystem::onSync() {
        if (context.getEngineContext().cameraRepository.movementSensitivity > 1.) {
            // TODO
        }
    }

    CameraSystem::CameraSystem(ApplicationContext &context) : AbstractRuntimeComponent(context) {}
}
