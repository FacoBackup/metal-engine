#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../ApplicationContext.h"
#include "../runtime/DescriptorInstance.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/FrameBufferAttachment.h"

namespace Metal {
    void CoreDescriptorSets::onInitialize() {
        {
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(static_cast<VkShaderStageFlagBits>(
                                                       VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT),
                                                   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            globalDataDescriptor->create(vulkanContext);
            globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                      vulkanContext.coreBuffers.globalData);
            globalDataDescriptor->write(vulkanContext);
        }

        {
            gBufferShadingDescriptor = std::make_unique<DescriptorInstance>();
            gBufferShadingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
            gBufferShadingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
            gBufferShadingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
            gBufferShadingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
            gBufferShadingDescriptor->create(vulkanContext);
            gBufferShadingDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->attachments[0]->vkImageView);
            gBufferShadingDescriptor->addImageDescriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->attachments[1]->vkImageView);
            gBufferShadingDescriptor->addImageDescriptor(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->attachments[2]->vkImageView);
            gBufferShadingDescriptor->addImageDescriptor(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                                context.vulkanContext.coreFrameBuffers.gBufferFBO->attachments[3]->vkImageView);
            gBufferShadingDescriptor->write(vulkanContext);
        }

        {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
            postProcessingDescriptor->create(vulkanContext);
            postProcessingDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                          vulkanContext.coreBuffers.postProcessingSettings);
            postProcessingDescriptor->addImageDescriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                context.vulkanContext.coreFrameBuffers.auxFBO->vkImageSampler,
                                                                context.vulkanContext.coreFrameBuffers.auxFBO->attachments[0]->vkImageView);
            postProcessingDescriptor->write(vulkanContext);
        }
    }
} // Metal
