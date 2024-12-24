#include "PipelineInstance.h"

#include "../descriptor/DescriptorInstance.h"
#include "../framebuffer/FrameBufferInstance.h"

namespace Metal {
    void PipelineInstance::dispose(VulkanContext &context) {
        std::cout << "Disposing of pipeline instance" << std::endl;

        vkDestroyPipelineLayout(context.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(context.device.device, vkPipeline, nullptr);
    }
}
