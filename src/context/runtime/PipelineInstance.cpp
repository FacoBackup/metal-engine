#include "PipelineInstance.h"

#include "BufferInstance.h"
#include "DescriptorInstance.h"
#include "MeshInstance.h"
#include "FrameBufferInstance.h"

namespace Metal {
    void PipelineInstance::dispose(VulkanContext &context) {
        vkDestroyPipelineLayout(context.device.device, vkPipelineLayout, nullptr);
        vkDestroyPipeline(context.device.device, vkPipeline, nullptr);
    }
}
