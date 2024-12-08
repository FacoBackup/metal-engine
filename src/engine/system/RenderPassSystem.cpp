#include "RenderPassSystem.h"

namespace Metal {
    void RenderPassSystem::render(VulkanFrameData &frameData) {
        for (auto *pass : renderPasses) {
            pass->onSync();
            pass->render(frameData);
        }
    }
} // Metal