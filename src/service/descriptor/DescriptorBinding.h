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
        std::shared_ptr<BufferInstance> bufferInstance = nullptr;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
        VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int bindingPoint,
                                    VkSampler sampler, VkImageView view,
                                    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL) {
            return DescriptorBinding(stageFlags, type, bindingPoint, nullptr, sampler, view, layout);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int bindingPoint,
                                    const std::shared_ptr<BufferInstance> &bufferInstance) {
            return DescriptorBinding(stageFlags, type, bindingPoint, bufferInstance);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int bindingPoint) {
            return DescriptorBinding(stageFlags, type, bindingPoint);
        }

        static DescriptorBinding OfAccelerationStructure(VkShaderStageFlagBits stageFlags, unsigned int bindingPoint,
                                                          VkAccelerationStructureKHR as) {
            DescriptorBinding b(stageFlags, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, bindingPoint);
            b.accelerationStructure = as;
            return b;
        }
    };
}

#endif //DESCRIPTORBINDING_H
