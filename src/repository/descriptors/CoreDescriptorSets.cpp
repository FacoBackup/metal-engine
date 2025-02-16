#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define GBUFFER_D(P, N)\
P = std::make_unique<DescriptorInstance>();\
P->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, context.coreFrameBuffers.gBufferFBO->vkImageSampler, context.coreFrameBuffers.gBufferFBO->attachments[N]->vkImageView));\
P->create(vulkanContext);

#define G context.coreFrameBuffers.gBufferFBO->attachments
#define COMPUTE_FRAGMENT_STAGES static_cast<VkShaderStageFlagBits>(VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
#define ALL_STAGES static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_COMPUTE_BIT)

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

    void CoreDescriptorSets::createSVOData() {
        svoData = std::make_unique<DescriptorInstance>();
        // ONE FOR EACH ADJACENT TILE AND ONE FOR THE CENTER TILE

        svoData->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                        0, context.coreBuffers.tileInfo));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 6));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 7));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8));
        svoData->addLayoutBinding(
            DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 9));

        svoData->create(vulkanContext);
    }

    void CoreDescriptorSets::createBuffersDescriptors() {
        rtTrianglesData = std::make_unique<DescriptorInstance>();
        rtTrianglesData->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0));
        rtTrianglesData->create(vulkanContext);

        rtBLASData = std::make_unique<DescriptorInstance>();
        rtBLASData->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0));
        rtBLASData->create(vulkanContext);

        rtTLASData = std::make_unique<DescriptorInstance>();
        rtTLASData->addLayoutBinding(
            DescriptorBinding::Of(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0));
        rtTLASData->create(vulkanContext);

        createSVOData();

        lightVolumeData = std::make_unique<DescriptorInstance>();
        lightVolumeData->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
                                                                context.coreBuffers.lightVolumeBuffer));
        lightVolumeData->create(vulkanContext);


        globalDataDescriptor = std::make_unique<DescriptorInstance>();
        globalDataDescriptor->addLayoutBinding(
            DescriptorBinding::Of(ALL_STAGES, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                  context.coreBuffers.globalData));
        globalDataDescriptor->create(vulkanContext);
    }

    void CoreDescriptorSets::createGBufferDescriptors() {
        gBufferAlbedo = std::make_unique<DescriptorInstance>();
        gBufferAlbedo->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                              G[0]->vkImageView));
        gBufferAlbedo->create(vulkanContext);

        gBufferNormal = std::make_unique<DescriptorInstance>();
        gBufferNormal->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                              context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                              G[1]->vkImageView));
        gBufferNormal->create(vulkanContext);

        gBufferPosition = std::make_unique<DescriptorInstance>();
        gBufferPosition->addLayoutBinding(DescriptorBinding::Of(COMPUTE_FRAGMENT_STAGES,
                                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                                G[2]->vkImageView));
        gBufferPosition->create(vulkanContext);
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
        }
    }

    void CoreDescriptorSets::onInitialize() {
        createBuffersDescriptors();
        createMaterialDescriptors();
        createGBufferDescriptors();
        createPathTracingDescriptors();

        if (context.isDebugMode()) {
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                                                         context.coreFrameBuffers.gBufferFBO->
                                                                         vkImageSampler,
                                                                         context.coreTextures.giSurfaceCache->
                                                                         vkImageView));
            surfaceCacheFragment->create(vulkanContext);
        } {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             context.coreFrameBuffers.gBufferFBO->
                                                                             vkImageSampler,
                                                                             context.coreTextures.currentFrame->
                                                                             vkImageView));
            postProcessingDescriptor->create(vulkanContext);
        }
    }
} // Metal
