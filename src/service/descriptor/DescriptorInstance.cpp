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
        std::vector<VkWriteDescriptorSet> writeDescriptorSets{};
        for (auto &binding: bindings) {
            if (binding.bufferInstance != nullptr) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = binding.bufferInstance->vkBuffer;
                bufferInfo.range = binding.bufferInstance->dataSize;
                bufferInfo.offset = 0;

                auto &descriptorWrite = writeDescriptorSets.emplace_back();
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfo;
            } else if (binding.view != VK_NULL_HANDLE) {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageView = binding.view;
                imageInfo.imageLayout = binding.layout;
                imageInfo.sampler = binding.sampler;

                auto &descriptorWrite = writeDescriptorSets.emplace_back();
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &imageInfo;
            }
        }

        vkUpdateDescriptorSets(context.device.device,
                               writeDescriptorSets.size(),
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

    void DescriptorInstance::addLayoutBinding(DescriptorBinding binding) {
        bindings.push_back(binding);
    }
}
