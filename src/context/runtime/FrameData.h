#ifndef VULKANFRAMEDATA_H
#define VULKANFRAMEDATA_H
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Metal {
    struct FrameData {
        std::vector<VkCommandBuffer> commandBuffers{};
    };
}

#endif
