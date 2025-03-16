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
    void CoreDescriptorSets::createBuffersDescriptors() {
        rtDescriptorSet = std::make_unique<DescriptorInstance>();
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                  context.coreBuffers.globalData));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                  context.coreBuffers.rtTrianglesBuffer));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2,
                                  context.coreBuffers.rtBLASBuffer));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3,
                                  context.coreBuffers.rtTLASBuffer));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4,
                                  context.coreBuffers.rtMaterialData));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 5,
                                  VK_NULL_HANDLE,
                                  context.coreTextures.currentFrame->
                                  vkImageView));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 6,
                                  VK_NULL_HANDLE,
                                  context.coreTextures.
                                  currentPositions->
                                  vkImageView));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 7,
                                  VK_NULL_HANDLE,
                                  context.coreTextures.giSurfaceCache->
                                  vkImageView));
        rtDescriptorSet->create(vulkanContext);

        lightsBuffer = std::make_unique<DescriptorInstance>();
        lightsBuffer->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                             context.coreBuffers.lightsBuffer));
        lightsBuffer->create(vulkanContext);

        volumesBuffer = std::make_unique<DescriptorInstance>();
        volumesBuffer->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                              VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                              context.coreBuffers.volumesBuffer));
        volumesBuffer->create(vulkanContext);

        globalDataDescriptor = std::make_unique<DescriptorInstance>();
        globalDataDescriptor->addLayoutBinding(
            DescriptorBinding::Of(ALL_STAGES, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                  context.coreBuffers.globalData));
        globalDataDescriptor->create(vulkanContext);
    }

    void CoreDescriptorSets::createPathTracingDescriptors() { {
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
        } {
            volumetricCurrentFrame = std::make_unique<DescriptorInstance>();
            volumetricCurrentFrame->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                           VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                           VK_NULL_HANDLE,
                                                                           context.coreTextures.volumetricCurrentFrame->
                                                                           vkImageView));
            volumetricCurrentFrame->create(vulkanContext);
        } {
            currentPositionsDescriptor = std::make_unique<DescriptorInstance>();
            currentPositionsDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                               VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                               VK_NULL_HANDLE,
                                                                               context.coreTextures.
                                                                               currentPositions->
                                                                               vkImageView));
            currentPositionsDescriptor->create(vulkanContext);
        } {
            accumulationMetadataDescriptor = std::make_unique<DescriptorInstance>();
            accumulationMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                VK_NULL_HANDLE,
                context.coreTextures.
                currentFrame->
                vkImageView));
            accumulationMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1,
                VK_NULL_HANDLE,
                context.coreTextures.
                volumetricCurrentFrame->
                vkImageView));
            accumulationMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2,
                VK_NULL_HANDLE,
                context.coreTextures.
                previousFrame->
                vkImageView));
            accumulationMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3,
                VK_NULL_HANDLE,
                context.coreTextures.
                previousFrameMetadata->
                vkImageView));
            accumulationMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4,
                VK_NULL_HANDLE,
                context.coreTextures.
                currentPositions->
                vkImageView));
            accumulationMetadataDescriptor->create(vulkanContext);
        } {
            currentPositionsFragmentDescriptor = std::make_unique<DescriptorInstance>();
            currentPositionsFragmentDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                vkImageSampler,
                context.coreTextures.
                currentPositions->
                vkImageView));
            currentPositionsFragmentDescriptor->create(vulkanContext);
        }
    }

    void CoreDescriptorSets::onInitialize() {
        context.framebufferService.createSampler(false, vkImageSampler);
        createBuffersDescriptors();
        createPathTracingDescriptors(); {
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
