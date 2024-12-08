#include "VoxelVisualizerRenderPass.h"
#include "../../../common/runtime/ApplicationContext.h"
#include "../../../common/util/VulkanUtils.h"
#include "../../service/core/framebuffer/FrameBufferInstance.h"
#include "../../service/core/pipeline/PipelineInstance.h"
#include "../../service/core/pools/CommandBufferInstance.h"

namespace Metal {
    void VoxelVisualizerRenderPass::render(VulkanFrameData &frameData) {
        VkCommandBuffer buffer = pipelines.debugPipeline->commandBuffer->vkBuffer;
        vkCmdBeginRenderPass(buffer, &pipelines.debugPipeline->commandBuffer->renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        BindPipeline(pipelines.debugPipeline);
        vkCmdEndRenderPass(buffer);
        VulkanUtils::CheckVKResult(vkEndCommandBuffer(buffer));
    }
} // Metal