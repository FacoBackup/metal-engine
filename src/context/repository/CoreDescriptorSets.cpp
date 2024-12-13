#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../ApplicationContext.h"
#include "../runtime/DescriptorInstance.h"
#include "../runtime/BufferInstance.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/FrameBufferAttachment.h"

namespace Metal {
    void CoreDescriptorSets::onInitialize() {
        DescriptorService &service = vulkanContext.descriptorService;

        globalDataDescriptor = service.createDescriptor();
        service.addLayoutBinding(globalDataDescriptor,
                                 static_cast<VkShaderStageFlagBits>(
                                     VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT),
                                 VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
        globalDataDescriptor->createLayout(vulkanContext);

        imageSampler = service.createDescriptor();
        service.addLayoutBinding(imageSampler, VK_SHADER_STAGE_FRAGMENT_BIT,
                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        imageSampler->createLayout(vulkanContext);
    }

    void CoreDescriptorSets::createDescriptors() {
        const auto pool = vulkanContext.descriptorPool;

        globalDataDescriptor->create(vulkanContext, pool);
        globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                  vulkanContext.coreBuffers.globalData);
        globalDataDescriptor->write(vulkanContext);


        imageSampler->create(vulkanContext, pool);
        updateImageSamplerDescriptor(vulkanContext.coreFrameBuffers.auxRenderPass->
                                     attachments[0]->vkImageSampler,
                                     vulkanContext.coreFrameBuffers.auxRenderPass->
                                     attachments[0]->vkImageView);
    }

    void CoreDescriptorSets::updateImageSamplerDescriptor(VkSampler sampler, VkImageView view) {
        if (sampler != lastSampler) {
            lastSampler = sampler;
            imageSampler->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                             sampler,
                                             view);
            imageSampler->write(vulkanContext);
        }
    }
} // Metal
