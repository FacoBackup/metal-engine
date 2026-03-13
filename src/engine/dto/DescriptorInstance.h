#ifndef DESCRIPTORINSTANCE_H
#define DESCRIPTORINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../resource/RuntimeResource.h"
#include "DescriptorBinding.h"

namespace Metal {
    struct BufferInstance;

    struct DescriptorInstance final : RuntimeResource {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        std::vector<DescriptorBinding> bindings;

        explicit DescriptorInstance(const std::string &id) : RuntimeResource(id) {
        }

        ResourceType resourceType() override {
            return DESCRIPTOR_SET;
        }
    };
} // Metal

#endif //DESCRIPTORINSTANCE_H
