#ifndef DESCRIPTORINSTANCE_H
#define DESCRIPTORINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/abstract/RuntimeResource.h"
#include "./DescriptorBinding.h"

namespace Metal {
    struct BufferInstance;

    struct DescriptorInstance final {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        std::vector<DescriptorBinding> bindings;

        void dispose(const VulkanContext &context) const;

        static void Write(const VulkanContext &context, const VkDescriptorSet &vkDescriptorSet, std::vector<DescriptorBinding> &bindings);

        void create(const VulkanContext &context);

        void addLayoutBinding(const DescriptorBinding &binding);
    };
} // Metal

#endif //DESCRIPTORINSTANCE_H
