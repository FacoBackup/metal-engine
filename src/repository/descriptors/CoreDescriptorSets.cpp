#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define G ApplicationContext::Get().coreFrameBuffers.gBufferFBO->attachments

namespace Metal {
    void CoreDescriptorSets::createMaterialDescriptors() {
        materialAlbedo = std::make_unique<DescriptorInstance>();
        materialNormal = std::make_unique<DescriptorInstance>();
        materialRoughness = std::make_unique<DescriptorInstance>();
        materialMetallic = std::make_unique<DescriptorInstance>();
        materialHeight = std::make_unique<DescriptorInstance>();

        materialAlbedo->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0));
        materialNormal->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0));
        materialRoughness->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0));
        materialMetallic->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0));
        materialHeight->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0));

        materialAlbedo->create();
        materialNormal->create();
        materialRoughness->create();
        materialMetallic->create();
        materialHeight->create();
    }

    void CoreDescriptorSets::createBuffersDescriptors() {
    {
            svoData = std::make_unique<DescriptorInstance>();
            // ONE FOR EACH ADJACENT TILE AND ONE FOR THE CENTER TILE

            svoData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                            0, ApplicationContext::Get().coreBuffers.tileInfo));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 6));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 7));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8));
            svoData->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 9));

            svoData->create();
        } {
            lightData = std::make_unique<DescriptorInstance>();
            lightData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    ApplicationContext::Get().coreBuffers.lightBuffer));
            lightData->create();
        } {
            volumeData = std::make_unique<DescriptorInstance>();
            volumeData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    ApplicationContext::Get().coreBuffers.volumesBuffer));
            volumeData->create();
        } {
            materialData = std::make_unique<DescriptorInstance>();
            materialData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    ApplicationContext::Get().coreBuffers.materialBuffer));
            materialData->create();
        } {
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                      ApplicationContext::Get().coreBuffers.globalData));
            globalDataDescriptor->create();
        }
    }

    void CoreDescriptorSets::createGBufferDescriptors() {
        gBufferAlbedo = std::make_unique<DescriptorInstance>();
        gBufferAlbedo->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              ApplicationContext::Get().coreDescriptorSets.vkImageSampler,
                                                              G[0]->vkImageView));
        gBufferAlbedo->create();

        gBufferNormal = std::make_unique<DescriptorInstance>();
        gBufferNormal->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              ApplicationContext::Get().coreDescriptorSets.vkImageSampler,
                                                              G[1]->vkImageView));
        gBufferNormal->create();

        gBufferPosition = std::make_unique<DescriptorInstance>();
        gBufferPosition->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                ApplicationContext::Get().coreDescriptorSets.vkImageSampler,
                                                                G[2]->vkImageView));
        gBufferPosition->create();
    }

    void CoreDescriptorSets::createPathTracingDescriptors() {
        {
            surfaceCacheImage = std::make_unique<DescriptorInstance>();
            surfaceCacheImage->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                          VK_NULL_HANDLE,
                                                                          ApplicationContext::Get().coreTextures.giSurfaceCache->
                                                                          vkImageView));
            surfaceCacheImage->create();
        }

        {
            previousFrameDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                            VK_NULL_HANDLE,
                                                                            ApplicationContext::Get().coreTextures.previousFrame->
                                                                            vkImageView));
            previousFrameDescriptor->create();
        }

        {
            previousFrameMetadataDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(
                VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                0, VK_NULL_HANDLE,
                ApplicationContext::Get().coreTextures.previousFrameMetadata->vkImageView));
            previousFrameMetadataDescriptor->create();
        }

        {
            currentFrameDescriptor = std::make_unique<DescriptorInstance>();
            currentFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                           VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                           VK_NULL_HANDLE,
                                                                           ApplicationContext::Get().coreTextures.currentFrame->vkImageView));
            currentFrameDescriptor->create();
        }
    }

    void CoreDescriptorSets::onInitialize() {
        ApplicationContext::Get().framebufferService.createSampler(false, vkImageSampler);

        createBuffersDescriptors();
        createMaterialDescriptors();
        createGBufferDescriptors();
        createPathTracingDescriptors();

        if (ApplicationContext::Get().isDebugMode()) {
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                         ApplicationContext::Get().coreDescriptorSets.vkImageSampler,
                                                                         ApplicationContext::Get().coreTextures.giSurfaceCache->
                                                                         vkImageView));
            surfaceCacheFragment->create();
        }

        {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             ApplicationContext::Get().coreDescriptorSets.vkImageSampler,
                                                                             ApplicationContext::Get().coreTextures.currentFrame->
                                                                             vkImageView));
            postProcessingDescriptor->create();
        }
    }
} // Metal
