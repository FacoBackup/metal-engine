#include "PipelineInstance.h"

#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"
#include "../descriptor/DescriptorInstance.h"
#include "../framebuffer/FrameBufferInstance.h"

namespace Metal {
    void PipelineInstance::dispose(VulkanContext &context) {
        LOG_INFO_S("Disposing of pipeline instance");

        vkDestroyPipelineLayout(context.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(context.device.device, vkPipeline, nullptr);
    }
}
