#ifndef RGRESOURCESTATE_H
#define RGRESOURCESTATE_H

#include <vulkan/vulkan.h>

namespace Metal {
    /**
     * @struct RGResourceState
     * @brief Represents the current state of a resource in the Render Graph.
     */
    struct RGResourceState {
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkAccessFlags access = 0;
        VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    };

    /**
     * @enum RGResourceAccess
     * @brief Types of access for a resource in a pass.
     */
    enum class RGResourceAccess {
        READ,
        WRITE,
        READ_WRITE
    };
}

#endif // RGRESOURCESTATE_H
