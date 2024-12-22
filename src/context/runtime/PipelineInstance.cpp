#include "PipelineInstance.h"

#include "DescriptorInstance.h"
#include "FrameBufferInstance.h"

namespace Metal {
    void PipelineInstance::dispose(VulkanContext &context) {
        std::cout << "Disposing of pipeline instance" << std::endl;

        vkDestroyPipelineLayout(context.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(context.device.device, vkPipeline, nullptr);
    }
}
