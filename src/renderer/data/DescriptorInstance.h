#ifndef DESCRIPTORINSTANCE_H
#define DESCRIPTORINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../common/RuntimeResource.h"
#include "DescriptorBinding.h"

namespace Metal {
    struct BufferInstance;

    struct DescriptorInstance final {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        std::vector<DescriptorBinding> bindings;

        void dispose() const;

        static void Write(const VkDescriptorSet &vkDescriptorSet, std::vector<DescriptorBinding> &bindings);

        void create();

        void addLayoutBinding(const DescriptorBinding &binding);
    };
} // Metal

#endif //DESCRIPTORINSTANCE_H
