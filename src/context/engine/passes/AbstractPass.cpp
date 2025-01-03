#include "AbstractPass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../service/pipeline/PipelineInstance.h"

namespace Metal {
    AbstractPass::AbstractPass(ApplicationContext &context, bool isComputePass) : AbstractRuntimeComponent(context),
        worldRepository(context.worldRepository),
        streamingRepository(
            context.streamingRepository), isComputePass(isComputePass) {
    }

    void AbstractPass::recordPushConstant(const void *data) {
        vkCmdPushConstants(
            vkCommandBuffer,
            getPipeline()->vkPipelineLayout,
            isComputePass ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            getPipeline()->pushConstantsSize,
            data);
    }
} // Metal
