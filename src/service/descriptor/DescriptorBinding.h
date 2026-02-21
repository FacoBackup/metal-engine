#ifndef DESCRIPTORBINDING_H
#define DESCRIPTORBINDING_H

#include "vulkan/vulkan.h"
#include <memory>

namespace Metal {
    struct BufferInstance;

    struct DescriptorBinding {
        VkShaderStageFlagBits stageFlags;
        VkDescriptorType descriptorType;
        unsigned int bindingPoint;
        unsigned int descriptorCount = 1;
        std::shared_ptr<BufferInstance> bufferInstance = nullptr;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
        VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;

        DescriptorBinding(VkShaderStageFlagBits stageFlags, VkDescriptorType descriptorType, unsigned int bindingPoint,
                          unsigned int descriptorCount = 1,
                          std::shared_ptr<BufferInstance> bufferInstance = nullptr, VkSampler sampler = VK_NULL_HANDLE,
                          VkImageView view = VK_NULL_HANDLE, VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL,
                          VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE)
            : stageFlags(stageFlags),
              descriptorType(descriptorType),
              bindingPoint(bindingPoint),
              descriptorCount(descriptorCount),
              bufferInstance(std::move(bufferInstance)),
              sampler(sampler),
              view(view),
              layout(layout),
              accelerationStructure(accelerationStructure) {
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int bindingPoint,
                                    VkSampler sampler, VkImageView view,
                                    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL, unsigned int descriptorCount = 1) {
            return DescriptorBinding(stageFlags, type, bindingPoint, descriptorCount, nullptr, sampler, view, layout);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int bindingPoint,
                                    const std::shared_ptr<BufferInstance> &bufferInstance) {
            return DescriptorBinding(stageFlags, type, bindingPoint, 1, bufferInstance);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int bindingPoint) {
            return DescriptorBinding(stageFlags, type, bindingPoint);
        }

        static DescriptorBinding OfAccelerationStructure(VkShaderStageFlagBits stageFlags, unsigned int bindingPoint,
                                                          VkAccelerationStructureKHR as) {
            return DescriptorBinding(stageFlags, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, bindingPoint, 1, nullptr,
                                     VK_NULL_HANDLE, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, as);
        }
    };
}

#endif //DESCRIPTORBINDING_H
