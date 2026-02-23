#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define G CTX.coreFrameBuffers.gBufferFBO->attachments

namespace Metal {
    void CoreDescriptorSets::createBuffersDescriptors() {
     {
            lightData = std::make_unique<DescriptorInstance>();
            lightData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    CTX.coreBuffers.lightBuffer));
            lightData->create();
        } {
            volumeData = std::make_unique<DescriptorInstance>();
            volumeData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    CTX.coreBuffers.volumesBuffer));
            volumeData->create();
        } {
            materialData = std::make_unique<DescriptorInstance>();
            materialData->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                    CTX.coreBuffers.materialBuffer));
            materialData->create();
        } {
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(
                DescriptorBinding::Of(VK_SHADER_STAGE_ALL, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                      CTX.coreBuffers.globalData));
            globalDataDescriptor->create();
        }
    }

    void CoreDescriptorSets::createTextureArrayDescriptor() {
        textureArray = std::make_unique<DescriptorInstance>();
        textureArray->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                            vkImageSampler, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1000));
        textureArray->create();
    }

    void CoreDescriptorSets::createGBufferDescriptors() {
        gBufferAlbedo = std::make_unique<DescriptorInstance>();
        gBufferAlbedo->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              CTX.coreDescriptorSets.vkImageSampler,
                                                              G[0]->vkImageView));
        gBufferAlbedo->create();

        gBufferNormal = std::make_unique<DescriptorInstance>();
        gBufferNormal->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              CTX.coreDescriptorSets.vkImageSampler,
                                                              G[1]->vkImageView));
        gBufferNormal->create();

        gBufferPosition = std::make_unique<DescriptorInstance>();
        gBufferPosition->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                CTX.coreDescriptorSets.vkImageSampler,
                                                                G[2]->vkImageView));
        gBufferPosition->create();
    }

    void CoreDescriptorSets::createPathTracingDescriptors() {
        {
            surfaceCacheImage = std::make_unique<DescriptorInstance>();
            surfaceCacheImage->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                          VK_NULL_HANDLE,
                                                                          CTX.coreTextures.surfaceCache->
                                                                          vkImageView));
            surfaceCacheImage->create();
        }

        {
            currentFrameDescriptor = std::make_unique<DescriptorInstance>();
            currentFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                           VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                           VK_NULL_HANDLE,
                                                                           CTX.coreTextures.rawRenderedFrame->vkImageView));
            currentFrameDescriptor->create();
        }

        {
            accumulatedFrameDescriptor = std::make_unique<DescriptorInstance>();
            accumulatedFrameDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_ALL,
                                                                               VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
                                                                               VK_NULL_HANDLE,
                                                                               CTX.coreTextures.accumulatedFrame->vkImageView));
            accumulatedFrameDescriptor->create();
        }
    }

    void CoreDescriptorSets::onInitialize() {
        CTX.framebufferService.createSampler(false, vkImageSampler);

        createBuffersDescriptors();
        createTextureArrayDescriptor();
        createGBufferDescriptors();
        createPathTracingDescriptors();

        if (CTX.isDebugMode()) {
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                         CTX.coreDescriptorSets.vkImageSampler,
                                                                         CTX.coreTextures.surfaceCache->
                                                                         vkImageView));
            surfaceCacheFragment->create();
        }

        {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             CTX.coreDescriptorSets.vkImageSampler,
                                                                             CTX.coreTextures.accumulatedFrame->
                                                                             vkImageView));
            postProcessingDescriptor->create();
        }
    }
} // Metal
