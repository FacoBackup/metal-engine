#include "Engine.h"

namespace Metal {
    void Engine::onInitialize() {
    }

    void Engine::onSync() {
        cameraSystem.onSync();
    }

    CameraSystem &Engine::getCameraSystem() {
        return cameraSystem;
    }
}
