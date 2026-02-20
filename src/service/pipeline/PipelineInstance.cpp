#include "PipelineInstance.h"

#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"
#include "../descriptor/DescriptorInstance.h"
#include "../framebuffer/FrameBufferInstance.h"

namespace Metal {
    void PipelineInstance::dispose() {
        LOG_INFO("Disposing of pipeline instance");

        vkDestroyPipelineLayout(CTX.vulkanContext.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(CTX.vulkanContext.device.device, vkPipeline, nullptr);
    }
}
