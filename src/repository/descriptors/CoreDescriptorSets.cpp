#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define G context.coreFrameBuffers.gBufferFBO->attachments

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

        materialAlbedo->create(vulkanContext);
        materialNormal->create(vulkanContext);
        materialRoughness->create(vulkanContext);
        materialMetallic->create(vulkanContext);
        materialHeight->create(vulkanContext);
    }

    void CoreDescriptorSets::createBuffersDescriptors() {
    {
            svoData = std::make_unique<DescriptorInstance>();
            // ONE FOR EACH ADJACENT TILE AND ONE FOR THE CENTER TILE

            svoData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                            0, context.coreBuffers.tileInfo));
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

            svoData->create(vulkanContext);
        } {
            lightData = std::make_unique<DescriptorInstance>();
            lightData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    context.coreBuffers.lightBuffer));
            lightData->create(vulkanContext);
        } {
            volumeData = std::make_unique<DescriptorInstance>();
            volumeData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    context.coreBuffers.volumesBuffer));
            volumeData->create(vulkanContext);
        } {
            materialData = std::make_unique<DescriptorInstance>();
            materialData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    context.coreBuffers.materialBuffer));
            materialData->create(vulkanContext);
        } {
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                      context.coreBuffers.globalData));
            globalDataDescriptor->create(vulkanContext);
        }
    }

    void CoreDescriptorSets::createGBufferDescriptors() {
        gBufferAlbedo = std::make_unique<DescriptorInstance>();
        gBufferAlbedo->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              context.coreDescriptorSets.vkImageSampler,
                                                              G[0]->vkImageView));
        gBufferAlbedo->create(vulkanContext);

        gBufferNormal = std::make_unique<DescriptorInstance>();
        gBufferNormal->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              context.coreDescriptorSets.vkImageSampler,
                                                              G[1]->vkImageView));
        gBufferNormal->create(vulkanContext);

        gBufferPosition = std::make_unique<DescriptorInstance>();
        gBufferPosition->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                context.coreDescriptorSets.vkImageSampler,
                                                                G[2]->vkImageView));
        gBufferPosition->create(vulkanContext);
    }

    void CoreDescriptorSets::createPathTracingDescriptors() {
        {
            surfaceCacheImage = std::make_unique<DescriptorInstance>();
            surfaceCacheImage->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                          VK_NULL_HANDLE,
                                                                          context.coreTextures.giSurfaceCache->
                                                                          vkImageView));
            surfaceCacheImage->create(vulkanContext);
        }

        {
            previousFrameDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT,
                                                                            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                            VK_NULL_HANDLE,
                                                                            context.coreTextures.previousFrame->
                                                                            vkImageView));
            previousFrameDescriptor->create(vulkanContext);
        }

        {
            previousFrameMetadataDescriptor = std::make_unique<DescriptorInstance>();
            previousFrameMetadataDescriptor->addLayoutBinding(DescriptorBinding::Of(
                VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                0, VK_NULL_HANDLE,
                context.coreTextures.previousFrameMetadata->vkImageView));
            previousFrameMetadataDescriptor->create(vulkanContext);
        }

        {
            currentFrameDescriptor = std::make_unique<DescriptorInstance>();
            currentFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                           VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                           VK_NULL_HANDLE,
                                                                           context.coreTextures.currentFrame->vkImageView));
            currentFrameDescriptor->create(vulkanContext);
        }
    }

    void CoreDescriptorSets::onInitialize() {
        context.framebufferService.createSampler(false, vkImageSampler);

        createBuffersDescriptors();
        createMaterialDescriptors();
        createGBufferDescriptors();
        createPathTracingDescriptors();

        if (context.isDebugMode()) {
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                         context.coreDescriptorSets.vkImageSampler,
                                                                         context.coreTextures.giSurfaceCache->
                                                                         vkImageView));
            surfaceCacheFragment->create(vulkanContext);
        }

        {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             context.coreDescriptorSets.vkImageSampler,
                                                                             context.coreTextures.currentFrame->
                                                                             vkImageView));
            postProcessingDescriptor->create(vulkanContext);
        }
    }
} // Metal
