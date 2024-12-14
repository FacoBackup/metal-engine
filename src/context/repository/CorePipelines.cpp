#include "CorePipelines.h"

#include "MeshPushConstant.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/MeshData.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/DEBUG.vert",
            "../resources/shaders/DEBUG.frag",
            {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor},
            sizeof(MeshPushConstant),
            false,
            true);

        gridPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/tool/GRID.frag",
                {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor},
            0,
            true
        );

    }

    void CorePipelines::dispose() {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        debugPipeline->dispose(vulkanContext);
    }
} // Metal
