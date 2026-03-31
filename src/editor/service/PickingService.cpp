#include "PickingService.h"

#include "engine/resource/BufferInstance.h"
#include "engine/resource/TextureInstance.h"
#include "ApplicationContext.h"
#include "../../core/VulkanContext.h"
#include "engine/service/BufferService.h"
#include "engine/repository/WorldRepository.h"
#include "engine/dto/StaticGeometryComponent.h"
#include "engine/dto/TransformComponent.h"
#include "engine/resource/RenderTargetInstance.h"

namespace Metal {
    std::optional<entt::entity> PickingService::pickEntityFromGBuffer(std::shared_ptr<RenderTargetAttachment> &attachment, const uint32_t pixelX,
                                                                  const uint32_t pixelY) const {

        constexpr VkDeviceSize imageSize = 2 * sizeof(float);
        auto stagingBuffer = bufferService->createBuffer("stagingBuffer", imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkCommandBuffer commandBuffer = vulkanContext->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = attachment->vkImage;
        barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {static_cast<int32_t>(pixelX), static_cast<int32_t>(pixelY), 0};
        region.imageExtent = {1, 1, 1};

        vkCmdCopyImageToBuffer(commandBuffer, attachment->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               stagingBuffer->vkBuffer, 1, &region);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);

        vulkanContext->endSingleTimeCommands(commandBuffer);

        void *data = nullptr;
        vkMapMemory(vulkanContext->device.device, stagingBuffer->vkDeviceMemory, 0, imageSize, 0, &data);
        const auto *pixel = static_cast<const float *>(data);
        const float idValue = pixel[0]; // Render index is in the R channel (index 0)
        vkUnmapMemory(vulkanContext->device.device, stagingBuffer->vkDeviceMemory);
        bufferService->dispose("stagingBuffer");

        if (idValue <= 0.0f) {
            return std::nullopt;
        }

        const unsigned int renderIndex = static_cast<unsigned int>(idValue + 0.5f) - 1;

        auto &registry = worldRepository->registry;
        auto view = registry.view<StaticGeometryComponent>();
        for (auto entity: view) {
            auto &mesh = registry.get<StaticGeometryComponent>(entity);
            if (mesh.renderIndex == renderIndex) {
                return entity;
            }
        }

        return std::nullopt;
    }
} // Metal
