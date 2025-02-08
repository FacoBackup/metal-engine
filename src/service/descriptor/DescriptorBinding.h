#ifndef DESCRIPTORBINDING_H
#define DESCRIPTORBINDING_H

#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;

    struct DescriptorBinding {
        VkShaderStageFlagBits stageFlags;
        VkDescriptorType descriptorType;
        uint32_t bindingPoint;
        std::shared_ptr<BufferInstance> bufferInstance = nullptr;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, uint32_t bindingPoint,
                                    VkSampler sampler, VkImageView view,
                                    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL) {
            return DescriptorBinding(stageFlags, type, bindingPoint, nullptr, sampler, view, layout);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, uint32_t bindingPoint,
                                    const std::shared_ptr<BufferInstance> &bufferInstance) {
            return DescriptorBinding(stageFlags, type, bindingPoint, bufferInstance);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, uint32_t bindingPoint) {
            return DescriptorBinding(stageFlags, type, bindingPoint);
        }
    };
}

#endif //DESCRIPTORBINDING_H
