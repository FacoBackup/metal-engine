#include "EngineContext.h"

namespace Metal {
    void EngineContext::onInitialize() {
    }

    void EngineContext::onSync() {
        cameraSystem.onSync();
    }

    EngineContext::EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {}

}
