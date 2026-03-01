#include "AbstractComputePass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../service/pipeline/PipelineInstance.h"
#include "../../../service/texture/TextureInstance.h"
#include "../../../util/ImageUtils.h"

namespace Metal {
    AbstractComputePass::AbstractComputePass(const std::string &id) : AbstractPass(id, true) {
    }

    void AbstractComputePass::recordDispatch(const unsigned int groupX, const unsigned int groupY,
                                             const unsigned int groupZ) const {
        vkCmdDispatch(vkCommandBuffer, groupX, groupY, groupZ);
    }

    void AbstractComputePass::clearTexture(const VkImage &image, glm::vec4 clearColor) const {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                vkCommandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            VkClearColorValue color = {.float32 = {clearColor.x, clearColor.y, clearColor.z, clearColor.w}};
            VkImageSubresourceRange imageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
            vkCmdClearColorImage(vkCommandBuffer, image, layout, &color, 1,
                                 &imageSubresourceRange);

            // Transition back to SHADER_READ_ONLY_OPTIMAL so startWriting can transition from there
            VkImageMemoryBarrier barrier2{};
            barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier2.image = image;
            barrier2.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
            barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                vkCommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier2);
    }

    void AbstractComputePass::recordImageDispatch(const TextureInstance *image, const unsigned int threadCountX,
                                                  const unsigned int threadCountY) const {
        vkCmdDispatch(vkCommandBuffer, image->width / threadCountX, image->height / threadCountY, 1);
    }

    void AbstractComputePass::endWriting(VkImage &vkImage) const {
        // // Convert image layout to READ_ONLY_OPTIMAL before reading from it in fragment shader.
        VkImageMemoryBarrier write2ReadBarrier = ImageUtils::writeToReadOnlyBarrier(
            vkImage);

        VkPipelineStageFlags writeStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        if (pipelineInstance != nullptr && pipelineInstance->isRayTracing) {
            writeStage = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
        }

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            writeStage,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &write2ReadBarrier);
    }

    void AbstractComputePass::startWriting(VkImage &vkImage) const {
        VkImageMemoryBarrier transferToGeneral = ImageUtils::ReadOnlyToGeneralBarrier(
            vkImage);

        VkPipelineStageFlags writeStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        if (pipelineInstance != nullptr && pipelineInstance->isRayTracing) {
            writeStage = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
        }

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            writeStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &transferToGeneral);
    }

    void AbstractComputePass::syncWriting(VkImage &vkImage) const {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = vkImage;
        barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

        VkPipelineStageFlags writeStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        if (pipelineInstance != nullptr && pipelineInstance->isRayTracing) {
            writeStage = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
        }

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            writeStage,
            writeStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }
}
