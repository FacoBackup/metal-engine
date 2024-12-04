#include "Engine.h"

namespace Metal {
    void Engine::onInitialize() {
    }

    void Engine::onSync() {
        cameraSystem.onSync();
    }

    Engine::Engine(ApplicationContext &context) : AbstractSystem(context) {}

}
