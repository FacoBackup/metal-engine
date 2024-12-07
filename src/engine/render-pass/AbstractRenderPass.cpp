#include "AbstractRenderPass.h"

namespace Metal {
    void AbstractRenderPass::onSync() {
    }

    void AbstractRenderPass::onInitialize() {
        createCommandBufferInternal();
        createDescriptorSetInternal();
        createRenderPassInternal();
    }
} // Metal