#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../ApplicationContext.h"
#include "../runtime/DescriptorInstance.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/FrameBufferAttachment.h"

namespace Metal {
    void CoreDescriptorSets::onInitialize() {
        globalDataDescriptor = std::make_unique<DescriptorInstance>();
        globalDataDescriptor->addLayoutBinding(static_cast<VkShaderStageFlagBits>(
                                                   VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT),
                                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
        globalDataDescriptor->create(vulkanContext);
        globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                  vulkanContext.coreBuffers.globalData);
        globalDataDescriptor->write(vulkanContext);

        postProcessingDescriptor = std::make_unique<DescriptorInstance>();
        postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
        postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
        postProcessingDescriptor->create(vulkanContext);
        postProcessingDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                      vulkanContext.coreBuffers.postProcessingSettings);
        postProcessingDescriptor->addImageDescriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                            context.vulkanContext.coreFrameBuffers.gBufferFBO->attachments[1]->vkImageSampler,
                                                            context.vulkanContext.coreFrameBuffers.gBufferFBO->attachments[1]->vkImageView);
        postProcessingDescriptor->write(vulkanContext);
    }
} // Metal
