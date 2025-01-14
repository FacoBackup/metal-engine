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
            VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT); {
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
        } {
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

        gBufferPositionataAlbedo = std::make_unique<DescriptorInstance>();
        gBufferPositionataNormal = std::make_unique<DescriptorInstance>();
        gBufferPositionataRoughness = std::make_unique<DescriptorInstance>();
        gBufferPositionataMetallic = std::make_unique<DescriptorInstance>();
        gBufferPositionataAO = std::make_unique<DescriptorInstance>();
        gBufferPositionataHeight = std::make_unique<DescriptorInstance>();

        gBufferPositionataAlbedo->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        gBufferPositionataNormal->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        gBufferPositionataRoughness->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        gBufferPositionataMetallic->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        gBufferPositionataAO->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                0);
        gBufferPositionataHeight->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);

        gBufferPositionataAlbedo->create(vulkanContext);
        gBufferPositionataNormal->create(vulkanContext);
        gBufferPositionataRoughness->create(vulkanContext);
        gBufferPositionataMetallic->create(vulkanContext);
        gBufferPositionataAO->create(vulkanContext);
        gBufferPositionataHeight->create(vulkanContext);

        // G-BUFFER
        GBUFFER_D(gBufferAlbedo, 0)
        GBUFFER_D(gBufferNormal, 1)
        GBUFFER_D(gBufferPosition, 2)

        surfaceCacheCompute = std::make_unique<DescriptorInstance>();
        surfaceCacheCompute->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0);
        surfaceCacheCompute->create(vulkanContext);
        surfaceCacheCompute->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE,
                                                 context.coreTextures.giSurfaceCache->vkImageView);
        surfaceCacheCompute->write(vulkanContext);

        giCompute = std::make_unique<DescriptorInstance>();
        giCompute->addLayoutBinding(VK_SHADER_STAGE_COMPUTE_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0);
        giCompute->create(vulkanContext);
        giCompute->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE,
                                                 context.coreTextures.globalIllumination->vkImageView);
        giCompute->write(vulkanContext);

        {
            globalIlluminationDescriptor = std::make_unique<DescriptorInstance>();
            globalIlluminationDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                           VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
            globalIlluminationDescriptor->create(vulkanContext);
            globalIlluminationDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                             context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                             context.coreTextures.globalIllumination->vkImageView);
            globalIlluminationDescriptor->write(vulkanContext);
        }

        if (context.isDebugMode()){
            surfaceCacheFragment = std::make_unique<DescriptorInstance>();
            surfaceCacheFragment->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                           VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
            surfaceCacheFragment->create(vulkanContext);
            surfaceCacheFragment->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                             context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                             context.coreTextures.giSurfaceCache->vkImageView);
            surfaceCacheFragment->write(vulkanContext);
        }

        if (context.isDebugMode()) {
            iconsDescriptor = std::make_unique<DescriptorInstance>();
            iconsDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                              0);
            iconsDescriptor->create(vulkanContext);
            iconsDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                context.coreTextures.icons->vkImageView);
            iconsDescriptor->write(vulkanContext);
        }
        // POST PROCESSING
        {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
            postProcessingDescriptor->create(vulkanContext);
            postProcessingDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                                         context.coreFrameBuffers.shadingBuffer->attachments[0]->
                                                         vkImageView);
            postProcessingDescriptor->write(vulkanContext);
        }
    }
} // Metal
