#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define GBUFFER_D(P, N)\
P = std::make_unique<DescriptorInstance>();\
P->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);\
P->create(vulkanContext);\
P->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,\
                                              context.coreFrameBuffers.gBufferFBO->vkImageSampler,\
                                              context.coreFrameBuffers.gBufferFBO->attachments[N]->vkImageView);\
P->write(vulkanContext);


namespace Metal {
    void CoreDescriptorSets::onInitialize() {
        constexpr auto COMPUTE_FRAGMENT_STAGES = static_cast<VkShaderStageFlagBits>(
            VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
        {
            svoData = std::make_unique<DescriptorInstance>();
            // ONE FOR EACH ADJACENT TILE AND ONE FOR THE CENTER TILE

            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 6);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 7);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8);
            svoData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 9);

            svoData->create(vulkanContext);
            svoData->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         context.coreBuffers.tileInfo);
            svoData->write(vulkanContext);
        }

        {
            volumesData = std::make_unique<DescriptorInstance>();
            volumesData->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0);
            volumesData->create(vulkanContext);
            volumesData->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                         context.coreBuffers.volumes);
            volumesData->write(vulkanContext);
        }

        {
            lightsData = std::make_unique<DescriptorInstance>();
            lightsData->addLayoutBinding(COMPUTE_FRAGMENT_STAGES, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0);
            lightsData->create(vulkanContext);
            lightsData->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                            context.coreBuffers.lights);
            lightsData->write(vulkanContext);
        }
        // GLOBAL DATA UBO
        {
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(static_cast<VkShaderStageFlagBits>(
                                                       VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT |
                                                       VK_SHADER_STAGE_COMPUTE_BIT),
                                                   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            globalDataDescriptor->create(vulkanContext);
            globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                      context.coreBuffers.globalData);
            globalDataDescriptor->write(vulkanContext);
        }

        materialAlbedo = std::make_unique<DescriptorInstance>();
        materialNormal = std::make_unique<DescriptorInstance>();
        materialRoughness = std::make_unique<DescriptorInstance>();
        materialMetallic = std::make_unique<DescriptorInstance>();
        materialHeight = std::make_unique<DescriptorInstance>();

        materialAlbedo->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        materialNormal->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        materialRoughness->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        materialMetallic->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                           VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        materialHeight->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);

        materialAlbedo->create(vulkanContext);
        materialNormal->create(vulkanContext);
        materialRoughness->create(vulkanContext);
        materialMetallic->create(vulkanContext);
        materialHeight->create(vulkanContext);

        // G-BUFFER
        GBUFFER_D(gBufferAlbedo, 0)
        GBUFFER_D(gBufferNormal, 1)
        GBUFFER_D(gBufferPosition, 2)

        giSurfaceCacheCompute = std::make_unique<DescriptorInstance>();
        giSurfaceCacheCompute->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0);
        giSurfaceCacheCompute->create(vulkanContext);
        giSurfaceCacheCompute->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE,
                                                context.coreTextures.giSurfaceCache->vkImageView);
        giSurfaceCacheCompute->write(vulkanContext);

        previousFrameDescriptor = std::make_unique<DescriptorInstance>();
        previousFrameDescriptor->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0);
        previousFrameDescriptor->create(vulkanContext);
        previousFrameDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE,
                                                  context.coreTextures.previousFrame->vkImageView);
        previousFrameDescriptor->write(vulkanContext);

        previousFrameMetadataDescriptor = std::make_unique<DescriptorInstance>();
        previousFrameMetadataDescriptor->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0);
        previousFrameMetadataDescriptor->create(vulkanContext);
        previousFrameMetadataDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE,
                                                  context.coreTextures.previousFrameMetadata->vkImageView);
        previousFrameMetadataDescriptor->write(vulkanContext);

        currentFrameDescriptor = std::make_unique<DescriptorInstance>();
        currentFrameDescriptor->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0);
        currentFrameDescriptor->create(vulkanContext);
        currentFrameDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE,
                                           context.coreTextures.currentFrame->vkImageView);
        currentFrameDescriptor->write(vulkanContext);

        if (context.isDebugMode()) {
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
            surfaceCacheFragment->create(vulkanContext);
            surfaceCacheFragment->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                     context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                     context.coreTextures.giSurfaceCache->vkImageView);
            surfaceCacheFragment->write(vulkanContext);
        }

        // POST PROCESSING
        {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
            postProcessingDescriptor->create(vulkanContext);
            postProcessingDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                         context.coreTextures.currentFrame->vkImageView);
            postProcessingDescriptor->write(vulkanContext);
        }
    }
} // Metal
