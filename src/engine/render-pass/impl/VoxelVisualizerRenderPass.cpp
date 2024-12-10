#include "VoxelVisualizerRenderPass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../common/util/VulkanUtils.h"

namespace Metal {
    void VoxelVisualizerRenderPass::onSync() {
        registerCommandBuffer(pipelines.debugPipeline);
    }
} // Metal