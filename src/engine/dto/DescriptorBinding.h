#ifndef DESCRIPTORBINDING_H
#define DESCRIPTORBINDING_H

#include "vulkan/vulkan.h"
#include <memory>

namespace Metal {
    struct BufferInstance;

    enum class DescriptorBindingType {
        BUFFER,
        COMBINED_IMAGE_SAMPLER,
        STORAGE_IMAGE,
        ACCELERATION_STRUCTURE,
        FBO_ATTACHMENT
    };

    struct DescriptorBinding {
        VkShaderStageFlags stageFlags = 0;
        VkDescriptorType descriptorType = static_cast<VkDescriptorType>(0);
        unsigned int bindingPoint = 0;
        unsigned int descriptorCount = 1;
        std::string bufferId = "";
        std::string storageImageId = "";
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
        VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
        std::string frameBufferId = "";
        int attachmentIndex = -1;

        DescriptorBinding() = default;

        DescriptorBinding(VkShaderStageFlags stageFlags, VkDescriptorType descriptorType, unsigned int bindingPoint,
                          unsigned int descriptorCount = 1,
                          const std::string &bufferId = "",
                          VkSampler sampler = VK_NULL_HANDLE,
                          VkImageView view = VK_NULL_HANDLE, VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL,
                          VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE)
            : stageFlags(stageFlags),
              descriptorType(descriptorType),
              bindingPoint(bindingPoint),
              descriptorCount(descriptorCount),
              bufferId(bufferId),
              sampler(sampler),
              view(view),
              layout(layout),
              accelerationStructure(accelerationStructure) {
        }

        static DescriptorBinding Of(VkShaderStageFlags stageFlags, VkDescriptorType type, unsigned int bindingPoint,
                                    VkSampler sampler, VkImageView view,
                                    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL, unsigned int descriptorCount = 1) {
            return DescriptorBinding(stageFlags, type, bindingPoint, descriptorCount, "", sampler, view, layout);
        }

        static DescriptorBinding Of(VkShaderStageFlags stageFlags, VkDescriptorType type, unsigned int bindingPoint,
                                    const std::string &bufferId) {
            return DescriptorBinding(stageFlags, type, bindingPoint, 1, bufferId);
        }

        static DescriptorBinding Of(VkShaderStageFlags stageFlags, VkDescriptorType type, unsigned int bindingPoint) {
            return DescriptorBinding(stageFlags, type, bindingPoint);
        }

        static DescriptorBinding OfAccelerationStructure(VkShaderStageFlags stageFlags, unsigned int bindingPoint,
                                                          VkAccelerationStructureKHR as) {
            return DescriptorBinding(stageFlags, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, bindingPoint, 1,
                                     "", VK_NULL_HANDLE, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, as);
        }
    };
}

#endif //DESCRIPTORBINDING_H
