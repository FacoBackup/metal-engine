#include "DescriptorInstance.h"

#include <iostream>

#include "DescriptorBinding.h"
#include "../buffer/BufferInstance.h"
#include "../../context/vulkan/VulkanContext.h"
#include "../../util/VulkanUtils.h"

namespace Metal {
    void DescriptorInstance::dispose(const VulkanContext &context) const {
        std::cout << "Disposing of descriptor instance" << std::endl;
        vkDestroyDescriptorSetLayout(context.device.device, vkDescriptorSetLayout, nullptr);
    }

    void DescriptorInstance::Write(const VulkanContext &context, const VkDescriptorSet &vkDescriptorSet,
                                   std::vector<DescriptorBinding> &bindings) {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;

        // First pass: Collect all buffer and image infos
        for (auto &binding: bindings) {
            if (binding.bufferInstance != nullptr) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = binding.bufferInstance->vkBuffer;
                bufferInfo.range = binding.bufferInstance->dataSize;
                bufferInfo.offset = 0;
                bufferInfos.push_back(bufferInfo);
            } else if (binding.view != VK_NULL_HANDLE) {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageView = binding.view;
                imageInfo.imageLayout = binding.layout;
                imageInfo.sampler = binding.sampler;
                imageInfos.push_back(imageInfo);
            }
        }

        // Second pass: Create write descriptor sets using the collected infos
        size_t bufferIndex = 0;
        size_t imageIndex = 0;
        for (auto &binding: bindings) {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = vkDescriptorSet;
            descriptorWrite.dstBinding = binding.bindingPoint;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = binding.descriptorType;
            descriptorWrite.descriptorCount = 1;

            if (binding.bufferInstance != nullptr) {
                descriptorWrite.pBufferInfo = &bufferInfos[bufferIndex];
                bufferIndex++;
            } else if (binding.view != VK_NULL_HANDLE) {
                descriptorWrite.pImageInfo = &imageInfos[imageIndex];
                imageIndex++;
            }

            writeDescriptorSets.push_back(descriptorWrite);
        }

        vkUpdateDescriptorSets(context.device.device,
                               static_cast<uint32_t>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(),
                               0,
                               nullptr);
        bindings.clear();
    }

    void DescriptorInstance::create(const VulkanContext &context) {
        if (bindings.empty()) {
            throw std::runtime_error("No descriptor layout sets were created");
        }
        // CREATE
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        for (auto &binding: bindings) {
            VkDescriptorSetLayoutBinding &samplerLayoutBinding = descriptorSetLayoutBindings.emplace_back();
            samplerLayoutBinding.binding = binding.bindingPoint;
            samplerLayoutBinding.descriptorType = binding.descriptorType;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.stageFlags = binding.stageFlags;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorSetLayoutBindings.size();
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

        VulkanUtils::CheckVKResult(vkCreateDescriptorSetLayout(context.device.device, &layoutInfo,
                                                               nullptr,
                                                               &vkDescriptorSetLayout));

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = context.descriptorPool; // Created during setup
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkDescriptorSetLayout;

        VulkanUtils::CheckVKResult(vkAllocateDescriptorSets(context.device.device, &allocInfo, &vkDescriptorSet));

        // WRITE
        Write(context, vkDescriptorSet, bindings);
    }

    void DescriptorInstance::addLayoutBinding(const DescriptorBinding &binding) {
        bindings.push_back(binding);
    }
}
