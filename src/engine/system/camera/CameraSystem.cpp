#include "CameraSystem.h"
#include "../../../common/runtime/ApplicationContext.h"

namespace Metal {
    void CameraSystem::onSync() {
        if (context.getEngine().cameraRepository.movementSensitivity > 1.) {
            // TODO
        }
    }

    CameraSystem::CameraSystem(ApplicationContext &context) : AbstractSystem(context) {}
}
