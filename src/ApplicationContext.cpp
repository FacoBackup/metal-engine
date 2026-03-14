#include "ApplicationContext.h"

#include "common/serialization-definitions.h"
#include "common/LoggerUtil.h"

#include "engine/EngineContext.h"
#include "core/glfw/GLFWContext.h"

namespace Metal {
    ApplicationContext::ApplicationContext(bool debugMode) : debugMode(debugMode) {
    }

    void ApplicationContext::onInitialize() {
        for (auto &instance : instances) {
            instance->setDependencies(*this);
        }

        for (auto &instance : instances) {
            auto *init = dynamic_cast<IInit *>(instance.get());
            if (init) {
                init->onInitialize();
            }
        }
    }

    bool ApplicationContext::isDebugMode() const {
        return debugMode;
    }

    void ApplicationContext::dispose() {
        try {
            for (auto it = instances.rbegin(); it != instances.rend(); ++it) {
                auto *disposable = dynamic_cast<IDisposable *>(it->get());
                if (disposable) {
                    disposable->dispose();
                }
            }
        } catch (std::exception &e) {
            LOG_ERROR(e.what());
        }
    }
}
