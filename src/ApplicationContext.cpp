#include "ApplicationContext.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "common/serialization-definitions.h"
#include "common/LoggerUtil.h"

#include "engine/EngineContext.h"
#include "core/WindowService.h"

namespace Metal {
    ApplicationContext::ApplicationContext(bool debugMode) : debugMode(debugMode) {
    }

    void ApplicationContext::onInitialize() {
        for (auto &instance: instances) {
            if (instance.get() != this) {
                instance->setDependencies(*this);
            }
        }

        for (auto &instance: instances) {
            if (instance.get() != this) {
                auto *init = dynamic_cast<IInit *>(instance.get());
                if (init) {
                    init->onInitialize();
                }
            }
        }
    }

    bool ApplicationContext::isDebugMode() const {
        return debugMode;
    }

    void ApplicationContext::dispose() {
        for (auto it = instances.rbegin(); it != instances.rend(); ++it) {
            if (it->get() == this) {
                continue;
            }
            auto *disposable = dynamic_cast<IDisposable *>(it->get());
            try {
                if (disposable) {
                    disposable->dispose();
                }
            } catch (std::exception &e) {
                LOG_ERROR(e.what());
            }
        }
    }
}
