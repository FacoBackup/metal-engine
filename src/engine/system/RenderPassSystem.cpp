#include "RenderPassSystem.h"

namespace Metal {
    void RenderPassSystem::onSync() {
        for (auto *pass: renderPasses) {
            if (pass != nullptr) {
                pass->onSync();
            }
        }
    }
} // Metal
