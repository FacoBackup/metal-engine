#ifndef DESCRIPTORBINDING_H
#define DESCRIPTORBINDING_H
#include <memory>
#include "vulkan/vulkan.h"

namespace Metal {
    struct BufferInstance;

    struct DescriptorBinding {
        VkShaderStageFlagBits stageFlags;
        VkDescriptorType descriptorType;
        unsigned int  bindingPoint;
        std::shared_ptr<BufferInstance> bufferInstance = nullptr;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int  bindingPoint,
                                    VkSampler sampler, VkImageView view,
                                    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL) {
            return DescriptorBinding(stageFlags, type, bindingPoint, nullptr, sampler, view, layout);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int  bindingPoint,
                                    const std::shared_ptr<BufferInstance> &bufferInstance) {
            return DescriptorBinding(stageFlags, type, bindingPoint, bufferInstance);
        }

        static DescriptorBinding Of(VkShaderStageFlagBits stageFlags, VkDescriptorType type, unsigned int  bindingPoint) {
            return DescriptorBinding(stageFlags, type, bindingPoint);
        }
    };
}

#endif //DESCRIPTORBINDING_H
