#include "CorePipelines.h"

#include "MeshPushConstant.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/assets/MeshData.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/DEBUG.vert",
            "../resources/shaders/DEBUG.frag",
            {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor.get()},
            sizeof(MeshPushConstant),
            true,
            true);

        gridPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/tool/GRID.frag",
            {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor.get()},
            0,
            true
        );
    }

    void CorePipelines::dispose() const {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        debugPipeline->dispose(vulkanContext);
        gridPipeline->dispose(vulkanContext);
    }
} // Metal
