#include "EngineContext.h"

#include "../common/runtime/ApplicationContext.h"
#include "../common/util/VulkanUtils.h"
#include "service/core/framebuffer/FrameBufferAttachment.h"
#include "service/core/framebuffer/FrameBufferInstance.h"

namespace Metal {
    void EngineContext::dispose() const {
        for (auto &resource: resourceRepository.resources) {
            resource.second->dispose(context.getVulkanContext());
            delete resource.second;
        }
        vkDestroyDescriptorPool(context.getVulkanContext().device.device, poolRepository.descriptorPool,
                                nullptr);

        vkDestroyCommandPool(context.getVulkanContext().device.device, poolRepository.commandPool,
                             nullptr);
    }

    void EngineContext::onInitialize() {
        textureService.onInitialize();
        framebufferService.onInitialize();
        pipelineService.onInitialize();
        shaderService.onInitialize();
        cameraSystem.onInitialize();
        poolService.onInitialize();
        coreFrameBuffers.onInitialize();
        corePipelines.onInitialize();


        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 0; // Binding index in the shader
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1; // Single texture
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // Used in fragment shader
        samplerLayoutBinding.pImmutableSamplers = nullptr; // Set this if you're using immutable samplers

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1; // Number of bindings
        layoutInfo.pBindings = &samplerLayoutBinding;

        VulkanUtils::CheckVKResult(vkCreateDescriptorSetLayout(context.getVulkanContext().device.device, &layoutInfo,
                                                               nullptr,
                                                               &descriptorSetLayout));

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = poolRepository.descriptorPool; // Created during setup
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        VulkanUtils::CheckVKResult(
            vkAllocateDescriptorSets(context.getVulkanContext().device.device, &allocInfo, &descriptorSet));

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageView = coreFrameBuffers.auxRenderPass->attachments[0]->vkImageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler = coreFrameBuffers.auxRenderPass->attachments[0]->vkImageSampler;
        // Create a sampler for the texture

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(context.getVulkanContext().device.device, 1, &descriptorWrite, 0, nullptr);
    }

    void EngineContext::onSync() {
        cameraSystem.onSync();
        renderPassSystem.onSync();
    }

    ResourceRepository &EngineContext::getResourceRepository() {
        return resourceRepository;
    }
}
