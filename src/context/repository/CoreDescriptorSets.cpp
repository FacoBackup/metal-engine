#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../ApplicationContext.h"
#include "../runtime/DescriptorInstance.h"

namespace Metal {
    void CoreDescriptorSets::onInitialize() {
        globalDataDescriptor = std::make_unique<DescriptorInstance>();
        globalDataDescriptor->addLayoutBinding(static_cast<VkShaderStageFlagBits>(
                                                   VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT),
                                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
        globalDataDescriptor->createLayout(vulkanContext);
        globalDataDescriptor->create(vulkanContext);
        globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                  vulkanContext.coreBuffers.globalData);
        globalDataDescriptor->write(vulkanContext);
    }
} // Metal
