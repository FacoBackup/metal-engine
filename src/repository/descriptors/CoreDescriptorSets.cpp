#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define COMPUTE_FRAGMENT_STAGES static_cast<VkShaderStageFlagBits>(VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
#define ALL_STAGES static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_COMPUTE_BIT)

namespace Metal {
    void CoreDescriptorSets::createBuffersDescriptors() { {
            rtTrianglesData = std::make_unique<DescriptorInstance>();
            rtTrianglesData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                      context.coreBuffers.rtTrianglesBuffer));
            rtTrianglesData->create(vulkanContext);
        } {
            rtBLASData = std::make_unique<DescriptorInstance>();
            rtBLASData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                      context.coreBuffers.rtBLASBuffer));
            rtBLASData->create(vulkanContext);
        } {
            rtTLASData = std::make_unique<DescriptorInstance>();
            rtTLASData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                      context.coreBuffers.rtTLASBuffer));
            rtTLASData->create(vulkanContext);
        }

        lightVolumeData = std::make_unique<DescriptorInstance>();
        lightVolumeData->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                context.coreBuffers.lightVolumeBuffer));
        lightVolumeData->create(vulkanContext);

        materialDataDescriptor = std::make_unique<DescriptorInstance>();
        materialDataDescriptor->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                                       VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                       context.coreBuffers.materialData));
        materialDataDescriptor->create(vulkanContext);


        globalDataDescriptor = std::make_unique<DescriptorInstance>();
        globalDataDescriptor->addLayoutBinding(
            DescriptorBinding::Of(ALL_STAGES, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                  context.coreBuffers.globalData));
        globalDataDescriptor->create(vulkanContext);
    }

    void CoreDescriptorSets::createPathTracingDescriptors() { {
            giSurfaceCacheCompute = std::make_unique<DescriptorInstance>();
            giSurfaceCacheCompute->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                          VK_NULL_HANDLE,
                                                                          context.coreTextures.giSurfaceCache->
                                                                          vkImageView));
            giSurfaceCacheCompute->create(vulkanContext);
        } {
            previousFrameDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                            VK_NULL_HANDLE,
                                                                            context.coreTextures.previousFrame->
                                                                            vkImageView));
            previousFrameDescriptor->create(vulkanContext);
        } {
            previousFrameMetadataDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(
                VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                0, VK_NULL_HANDLE,
                context.coreTextures.previousFrameMetadata->vkImageView));
            previousFrameMetadataDescriptor->create(vulkanContext);
        } {
            currentFrameDescriptor = std::make_unique<DescriptorInstance>();
            currentFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                           VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                           VK_NULL_HANDLE,
                                                                           context.coreTextures.currentFrame->
                                                                           vkImageView));
            currentFrameDescriptor->create(vulkanContext);
        }{
            currentPositionsDescriptor = std::make_unique<DescriptorInstance>();
            currentPositionsDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                             VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                             VK_NULL_HANDLE,
                                                                             context.coreTextures.
                                                                             currentPositions->
                                                                             vkImageView));
            currentPositionsDescriptor->create(vulkanContext);
        }
    }

    void CoreDescriptorSets::onInitialize() {
        context.framebufferService.createSampler(false, vkImageSampler);
        createBuffersDescriptors();
        createPathTracingDescriptors();

        if (context.isDebugMode()) {
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                         vkImageSampler,
                                                                         context.coreTextures.giSurfaceCache->
                                                                         vkImageView));
            surfaceCacheFragment->create(vulkanContext);
        } {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             vkImageSampler,
                                                                             context.coreTextures.currentFrame->
                                                                             vkImageView));
            postProcessingDescriptor->create(vulkanContext);
        }
    }
} // Metal
