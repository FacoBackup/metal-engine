#include "VideoExporterService.h"
#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../util/FilesUtil.h"
#include "../../../dependencies/stb/stb_image_write.h"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>

namespace Metal {
    void VideoExporterService::cleanupFFmpeg() {
        if (frameOutputDirectory.empty()) {
            return;
        }

        std::error_code error;
        std::filesystem::remove_all(frameOutputDirectory, error);
        frameOutputDirectory.clear();
    }

    void VideoExporterService::exportToVideo() {
        if (isFirstBakeLoop) {
            frameOutputDirectory = ApplicationContext::Get().getRootDirectory() + "/video-frames";
            std::error_code error;
            std::filesystem::remove_all(frameOutputDirectory, error);
            FilesUtil::MkDir(frameOutputDirectory);

            ApplicationContext::Get().engineRepository.isBaking = true;
            isFirstBakeLoop = false;
            ApplicationContext::Get().notificationService.pushMessage("Starting baking process", NotificationSeverities::SUCCESS);
            frameCount = 0;
        } else {
            ApplicationContext::Get().notificationService.pushMessage("Baking already started", NotificationSeverities::WARNING);
        }
    }

    void VideoExporterService::finishExportToVideo() {
        if (!ApplicationContext::Get().engineRepository.isBaking) {
            return;
        }

        ApplicationContext::Get().engineRepository.isBaking = false;
        isFirstBakeLoop = true;
        ApplicationContext::Get().notificationService.pushMessage("Finishing baking process", NotificationSeverities::SUCCESS);

        if (!frameOutputDirectory.empty()) {
            const std::string outputPath = ApplicationContext::Get().getVideoOutputPath();
            const std::string framePattern = (std::filesystem::path(frameOutputDirectory) / "frame_%06d.png").string();
            const std::string command = "ffmpeg -y -framerate 30 -i \"" + framePattern +
                                        "\" -c:v libx264 -pix_fmt yuv420p \"" + outputPath + "\"";
            const int result = std::system(command.c_str());
            if (result != 0) {
                ApplicationContext::Get().notificationService.pushMessage("FFmpeg failed to assemble video",
                                                        NotificationSeverities::ERROR);
            } else {
                ApplicationContext::Get().notificationService.pushMessage("Video export finished", NotificationSeverities::SUCCESS);
            }
        }

        cleanupFFmpeg();
    }

    void VideoExporterService::onSync() {
        if (!ApplicationContext::Get().engineRepository.isBaking) {
            return;
        }

        if (ApplicationContext::Get().engineRepository.maxVideoFrames == frameCount) {
            finishExportToVideo();
            return;
        }

        if (ApplicationContext::Get().engineRepository.pathTracerAccumulationCount >= ApplicationContext::Get().engineRepository.pathTracerMaxSamples) {
            LOG_DEBUG("Starting frame write " + std::to_string(frameCount));

            auto *finalFrame = ApplicationContext::Get().coreFrameBuffers.postProcessingFBO;
            if (!finalFrame || finalFrame->attachments.empty()) return;

            auto &attachment = finalFrame->attachments[0];
            const auto width = finalFrame->bufferWidth;
            const auto height = finalFrame->bufferHeight;

            VkDeviceSize imageSize = width * height * 4;
            auto stagingBuffer = ApplicationContext::Get().bufferService.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            VkCommandBuffer commandBuffer = ApplicationContext::Get().vulkanContext.beginSingleTimeCommands();

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
            region.imageOffset = {0, 0, 0};
            region.imageExtent = {width, height, 1};

            vkCmdCopyImageToBuffer(commandBuffer, attachment->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   stagingBuffer->vkBuffer, 1, &region);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0, 0, nullptr, 0, nullptr, 1, &barrier);

            ApplicationContext::Get().vulkanContext.endSingleTimeCommands(commandBuffer);

            void *data;
            vkMapMemory(ApplicationContext::Get().vulkanContext.device.device, stagingBuffer->vkDeviceMemory, 0, imageSize, 0, &data);

            std::ostringstream frameName;
            frameName << frameOutputDirectory << "/frame_" << std::setw(6) << std::setfill('0') << frameCount
                      << ".png";
            if (!stbi_write_png(frameName.str().c_str(), static_cast<int>(width), static_cast<int>(height), 4, data,
                                static_cast<int>(width * 4))) {
                ApplicationContext::Get().notificationService.pushMessage("Failed to write PNG frame",
                                                        NotificationSeverities::ERROR);
            }

            vkUnmapMemory(ApplicationContext::Get().vulkanContext.device.device, stagingBuffer->vkDeviceMemory);
            stagingBuffer->dispose();

            frameCount++;

            ApplicationContext::Get().engineRepository.pathTracerAccumulationCount = 0;
        }
    }
} // Metal
