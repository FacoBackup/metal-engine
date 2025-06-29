#include "PipelineInstance.h"

#include "DescriptorInstance.h"
#include "FrameBufferInstance.h"
#include "../../MetalContextProvider.h"

namespace Metal {
    void PipelineInstance::dispose() {
        std::cout << "Disposing of pipeline instance" << std::endl;

        vkDestroyPipelineLayout(SINGLETONS.vulkanContext.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(SINGLETONS.vulkanContext.device.device, vkPipeline, nullptr);
    }
}
