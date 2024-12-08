#ifndef POOLREPOSITORY_H
#define POOLREPOSITORY_H
#include <vulkan/vulkan_core.h>

namespace Metal {
    struct CommandPoolRepository {
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };
} // Metal

#endif //POOLREPOSITORY_H
