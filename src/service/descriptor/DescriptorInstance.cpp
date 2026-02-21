#include "DescriptorInstance.h"

#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"

#include "DescriptorBinding.h"
#include "../buffer/BufferInstance.h"
#include "../../context/vulkan/VulkanContext.h"
#include "../../util/VulkanUtils.h"

namespace Metal {
    void DescriptorInstance::dispose() const {
        LOG_INFO("Disposing of descriptor instance");
        vkDestroyDescriptorSetLayout(CTX.vulkanContext.device.device, vkDescriptorSetLayout, nullptr);
    }

    void DescriptorInstance::Write(const VkDescriptorSet &vkDescriptorSet,
                                   std::vector<DescriptorBinding> &bindings) {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        // These vectors will hold the Vk*Info objects so their memory remains valid.
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<std::vector<VkDescriptorImageInfo>> imageInfosPool;
        std::vector<VkWriteDescriptorSetAccelerationStructureKHR> asInfos;

        for (auto &binding: bindings) {
            if (binding.accelerationStructure != VK_NULL_HANDLE) {
                VkWriteDescriptorSetAccelerationStructureKHR asInfo{};
                asInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
                asInfo.accelerationStructureCount = 1;
                asInfo.pAccelerationStructures = &binding.accelerationStructure;
                asInfos.push_back(asInfo);

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pNext = &asInfos.back();

                writeDescriptorSets.push_back(descriptorWrite);
            } else if (binding.bufferInstance != nullptr) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = binding.bufferInstance->vkBuffer;
                bufferInfo.range = binding.bufferInstance->dataSize;
                bufferInfo.offset = 0;
                // Store the buffer info in the vector.
                bufferInfos.push_back(bufferInfo);

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = 1;
                // Use the address of the element in the vector.
                descriptorWrite.pBufferInfo = &bufferInfos.back();

                writeDescriptorSets.push_back(descriptorWrite);
            } else if (binding.view != VK_NULL_HANDLE) {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageView = binding.view;
                imageInfo.imageLayout = binding.layout;
                imageInfo.sampler = binding.sampler;
                
                std::vector<VkDescriptorImageInfo> imageInfos;
                imageInfos.push_back(imageInfo);
                imageInfosPool.push_back(imageInfos);

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = binding.descriptorCount;
                descriptorWrite.pImageInfo = imageInfosPool.back().data();

                writeDescriptorSets.push_back(descriptorWrite);
            }
        }

        vkUpdateDescriptorSets(CTX.vulkanContext.device.device,
                               static_cast<unsigned int>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(),
                               0,
                               nullptr);
        bindings.clear();
    }

    void DescriptorInstance::create() {
        if (bindings.empty()) {
            throw std::runtime_error("No descriptor layout sets were created");
        }
        // CREATE
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        std::vector<VkDescriptorBindingFlags> bindingFlags;

        for (auto &binding: bindings) {
            VkDescriptorSetLayoutBinding &samplerLayoutBinding = descriptorSetLayoutBindings.emplace_back();
            samplerLayoutBinding.binding = binding.bindingPoint;
            samplerLayoutBinding.descriptorType = binding.descriptorType;
            samplerLayoutBinding.descriptorCount = binding.descriptorCount;
            samplerLayoutBinding.stageFlags = binding.stageFlags;
            samplerLayoutBinding.pImmutableSamplers = nullptr;

            if (binding.descriptorCount > 1) {
                bindingFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            } else {
                bindingFlags.push_back(0);
            }
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{};
        flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        flagsInfo.bindingCount = bindingFlags.size();
        flagsInfo.pBindingFlags = bindingFlags.data();

        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorSetLayoutBindings.size();
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
        layoutInfo.pNext = &flagsInfo;

        VulkanUtils::CheckVKResult(vkCreateDescriptorSetLayout(CTX.vulkanContext.device.device, &layoutInfo,
                                                               nullptr,
                                                               &vkDescriptorSetLayout));

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = CTX.vulkanContext.descriptorPool; // Created during setup
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkDescriptorSetLayout;

        VulkanUtils::CheckVKResult(vkAllocateDescriptorSets(CTX.vulkanContext.device.device, &allocInfo, &vkDescriptorSet));

        // WRITE
        Write(vkDescriptorSet, bindings);
    }

    void DescriptorInstance::addLayoutBinding(DescriptorBinding binding) {
        bindings.push_back(binding);
    }
}
