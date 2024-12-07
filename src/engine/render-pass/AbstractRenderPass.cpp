#include "AbstractRenderPass.h"

#include <vector>

namespace Metal {

    void AbstractRenderPass::onSync() {
    }

    void AbstractRenderPass::onInitialize() {
        createCommandBufferInternal();
        createDescriptorSetInternal();
        createRenderPassInternal();
        initializeFrameBuffers();
    }
} // Metal
