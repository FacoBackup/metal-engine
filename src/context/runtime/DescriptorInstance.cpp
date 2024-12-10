#include "DescriptorInstance.h"

#include "BufferInstance.h"
#include "../VulkanContext.h"
#include "../../common/util/VulkanUtils.h"

namespace Metal {
    void DescriptorInstance::dispose(VulkanContext &context) {
        if (ready)
            vkDestroyDescriptorSetLayout(context.device.device, vkDescriptorSetLayout, nullptr);
    }

    void DescriptorInstance::create(const VulkanContext &context, VkDescriptorPool pool) {
        if (descriptorSetLayoutBindings.empty()) {
            throw std::runtime_error("No descriptor layout sets were created");
        }
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorSetLayoutBindings.size();
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();

        VulkanUtils::CheckVKResult(vkCreateDescriptorSetLayout(context.device.device, &layoutInfo,
                                                               nullptr,
                                                               &vkDescriptorSetLayout));

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool; // Created during setup
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkDescriptorSetLayout;

        VulkanUtils::CheckVKResult(vkAllocateDescriptorSets(context.device.device, &allocInfo, &vkDescriptorSet));
        ready = true;
    }

    void DescriptorInstance::addBufferDescriptor(const uint32_t bindingPoint, VkDescriptorType type,
                                                 BufferInstance *bufferInstance) {
        if (!ready) {
            throw std::runtime_error("Descriptor instance is not ready");
        }

        auto &bufferInfo = bufferInfos.emplace_back();

        bufferInfo.buffer = bufferInstance->getBuffer();
        bufferInfo.range = VK_WHOLE_SIZE;
        bufferInfo.offset = 0;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = vkDescriptorSet;
        descriptorWrite.dstBinding = bindingPoint;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = type;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        writeDescriptorSets.push_back(descriptorWrite);
    }

    void DescriptorInstance::addImageDescriptor(const uint32_t bindingPoint, VkDescriptorType type, VkSampler sampler,
                                                VkImageView view) {
        if (!ready) {
            throw std::runtime_error("Descriptor instance is not ready");
        }

        auto &imageInfo = imageInfos.emplace_back();;
        imageInfo.imageView = view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler = sampler;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = vkDescriptorSet;
        descriptorWrite.dstBinding = bindingPoint;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = type;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        writeDescriptorSets.push_back(descriptorWrite);
    }

    void DescriptorInstance::write(const VulkanContext &context) {
        if (!ready) {
            throw std::runtime_error("Descriptor instance is not ready");
        }
        if (writeDescriptorSets.empty()) {
            throw std::runtime_error("No write descriptor sets were created");
        }

        vkUpdateDescriptorSets(context.device.device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0,
                               nullptr);
        imageInfos.clear();
        bufferInfos.clear();
    }
} // Metal
