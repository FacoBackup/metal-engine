#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../MetalContextProvider.h"
#include "../data/DescriptorBinding.h"
#include "../data/DescriptorInstance.h"
#include "../data/FrameBufferInstance.h"
#include "../data/FrameBufferAttachment.h"
#include "../data/TextureInstance.h"

#define COMPUTE_FRAGMENT_STAGES static_cast<VkShaderStageFlagBits>(VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
#define ALL_STAGES static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_COMPUTE_BIT)

namespace Metal {
    void CoreDescriptorSets::createBuffersDescriptors() {
        rtDescriptorSet = std::make_unique<DescriptorInstance>();
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                  SINGLETONS.coreBuffers.globalData));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                  SINGLETONS.coreBuffers.rtTrianglesBuffer));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2,
                                  SINGLETONS.coreBuffers.rtBLASBuffer));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3,
                                  SINGLETONS.coreBuffers.rtTLASBuffer));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4,
                                  SINGLETONS.coreBuffers.rtMaterialData));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 5,
                                  VK_NULL_HANDLE,
                                  SINGLETONS.coreTextures.currentFrame->
                                  vkImageView));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 6,
                                  VK_NULL_HANDLE,
                                  SINGLETONS.coreTextures.
                                  currentPositions->
                                  vkImageView));
        rtDescriptorSet->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 7,
                                  VK_NULL_HANDLE,
                                  SINGLETONS.coreTextures.giSurfaceCache->
                                  vkImageView));
        rtDescriptorSet->create();

        lightsBuffer = std::make_unique<DescriptorInstance>();
        lightsBuffer->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                             SINGLETONS.coreBuffers.lightsBuffer));
        lightsBuffer->create();

        volumesBuffer = std::make_unique<DescriptorInstance>();
        volumesBuffer->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                              VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                              SINGLETONS.coreBuffers.volumesBuffer));
        volumesBuffer->create();

        globalDataDescriptor = std::make_unique<DescriptorInstance>();
        globalDataDescriptor->addLayoutBinding(
            DescriptorBinding::Of(ALL_STAGES, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                  SINGLETONS.coreBuffers.globalData));
        globalDataDescriptor->create();
    }

    void CoreDescriptorSets::createPathTracingDescriptors() { {
            previousFrameDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                            VK_NULL_HANDLE,
                                                                            SINGLETONS.coreTextures.previousFrame->
                                                                            vkImageView));
            previousFrameDescriptor->create();
        } {
            previousFrameMetadataDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(
                VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                0, VK_NULL_HANDLE,
                SINGLETONS.coreTextures.previousFrameMetadata->vkImageView));
            previousFrameMetadataDescriptor->create();
        } {
            currentFrameDescriptor = std::make_unique<DescriptorInstance>();
            currentFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                           VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                           VK_NULL_HANDLE,
                                                                           SINGLETONS.coreTextures.currentFrame->
                                                                           vkImageView));
            currentFrameDescriptor->create();
        } {
            currentPositionsDescriptor = std::make_unique<DescriptorInstance>();
            currentPositionsDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                               VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                               VK_NULL_HANDLE,
                                                                               SINGLETONS.coreTextures.
                                                                               currentPositions->
                                                                               vkImageView));
            currentPositionsDescriptor->create();
        } {
            currentPositionsFragmentDescriptor = std::make_unique<DescriptorInstance>();
            currentPositionsFragmentDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                vkImageSampler,
                SINGLETONS.coreTextures.
                currentPositions->
                vkImageView));
            currentPositionsFragmentDescriptor->create();
        }
    }

    void CoreDescriptorSets::onInitialize() {
        SINGLETONS.framebufferService.createSampler(false, vkImageSampler);
        createBuffersDescriptors();
        createPathTracingDescriptors(); {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             vkImageSampler,
                                                                             SINGLETONS.coreTextures.currentFrame->
                                                                             vkImageView));
            postProcessingDescriptor->create();
        }
    }
} // Metal
