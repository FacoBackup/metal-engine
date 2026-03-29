#ifndef RGBARRIERBATCH_H
#define RGBARRIERBATCH_H

#include <vector>
#include <vulkan/vulkan.h>

namespace Metal {
    /**
     * @struct RGBarrierBatch
     * @brief A collection of pipeline barriers to be executed before a pass.
     */
    struct RGBarrierBatch {
        std::vector<VkImageMemoryBarrier> imageBarriers;
        std::vector<VkBufferMemoryBarrier> bufferBarriers;
        VkPipelineStageFlags srcStageMask = 0;
        VkPipelineStageFlags dstStageMask = 0;

        void addImageBarrier(const VkImageMemoryBarrier& barrier, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) {
            imageBarriers.push_back(barrier);
            srcStageMask |= srcStage;
            dstStageMask |= dstStage;
        }

        void addBufferBarrier(const VkBufferMemoryBarrier& barrier, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) {
            bufferBarriers.push_back(barrier);
            srcStageMask |= srcStage;
            dstStageMask |= dstStage;
        }

        [[nodiscard]] bool isEmpty() const {
            return imageBarriers.empty() && bufferBarriers.empty();
        }

        void execute(VkCommandBuffer cmd) const {
            if (isEmpty()) return;

            vkCmdPipelineBarrier(
                cmd,
                srcStageMask,
                dstStageMask,
                0,
                0, nullptr,
                static_cast<uint32_t>(bufferBarriers.size()), bufferBarriers.data(),
                static_cast<uint32_t>(imageBarriers.size()), imageBarriers.data()
            );
        }
    };
}

#endif // RGBARRIERBATCH_H
