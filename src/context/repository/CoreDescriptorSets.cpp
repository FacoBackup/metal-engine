#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../ApplicationContext.h"
#include "../runtime/DescriptorInstance.h"
#include "../runtime/BufferInstance.h"
#include "../runtime/FrameBufferInstance.h"
#include "../runtime/FrameBufferAttachment.h"

namespace Metal {
    void CoreDescriptorSets::onInitialize() {
        const DescriptorService &service = context.getVulkanContext().descriptorService;
        currentFrameImageDescriptor = service.createDescriptor();
        service.addLayoutBinding(currentFrameImageDescriptor, VK_SHADER_STAGE_FRAGMENT_BIT,
                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);


        debugDescriptor = service.createDescriptor();
        service.addLayoutBinding(debugDescriptor, VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
    }

    void CoreDescriptorSets::createDescriptors() const {
        const auto pool = context.getVulkanContext().descriptorPool;

        debugDescriptor->create(context.getVulkanContext(), pool);
        debugDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                             context.getVulkanContext().coreBuffers.globalData);
        debugDescriptor->write(context.getVulkanContext());


        currentFrameImageDescriptor->create(context.getVulkanContext(), pool);
        currentFrameImageDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                        context.getVulkanContext().coreFrameBuffers.auxRenderPass->
                                                        attachments[0]->vkImageSampler,
                                                        context.getVulkanContext().coreFrameBuffers.auxRenderPass->
                                                        attachments[0]->vkImageView);
        currentFrameImageDescriptor->write(context.getVulkanContext());
    }
} // Metal
