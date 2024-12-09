#include "VoxelVisualizerRenderPass.h"
#include "../../../common/runtime/ApplicationContext.h"
#include "../../../common/util/VulkanUtils.h"
#include "../../service/core/framebuffer/FrameBufferInstance.h"
#include "../../service/core/pipeline/PipelineInstance.h"
#include "../../service/core/pools/CommandBufferInstance.h"

namespace Metal {
    void VoxelVisualizerRenderPass::onSync() {
        context.getVulkanContext().getFrameData().commandBuffers.push_back(pipelines.debugPipeline->commandBuffer->vkCommandBuffer);
    }
} // Metal