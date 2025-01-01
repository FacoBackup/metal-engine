#include "CoreDescriptorSets.h"

#include <vulkan/vulkan_core.h>
#include "../../context/ApplicationContext.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

#define GBUFFER_D(P, N)\
P = std::make_unique<DescriptorInstance>();\
P->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);\
P->create(vulkanContext);\
P->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,\
                                              context.coreFrameBuffers.gBufferFBO->vkImageSampler,\
                                              context.coreFrameBuffers.gBufferFBO->attachments[N]->vkImageView);\
P->write(vulkanContext);


namespace Metal {
    void CoreDescriptorSets::onInitialize() { {
            svoData = std::make_unique<DescriptorInstance>();
            // ONE FOR EACH ADJACENT TILE AND ONE FOR THE CENTER TILE
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 6);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 7);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8);
            svoData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 9);

            svoData->create(vulkanContext);
            svoData->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         context.coreBuffers.tileInfo);
            svoData->write(vulkanContext);
        } {
            lightsData = std::make_unique<DescriptorInstance>();
            lightsData->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0);
            lightsData->create(vulkanContext);
            lightsData->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                            context.coreBuffers.lights);
            lightsData->write(vulkanContext);
        }
        // GLOBAL DATA UBO
        {
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(static_cast<VkShaderStageFlagBits>(
                                                       VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT),
                                                   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            globalDataDescriptor->create(vulkanContext);
            globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                      context.coreBuffers.globalData);
            globalDataDescriptor->write(vulkanContext);
        }

        // G-BUFFER
        GBUFFER_D(gBufferAlbedo, 0)
        GBUFFER_D(gBufferShadingRMAO, 1)
        GBUFFER_D(gBufferNormal, 2)
        GBUFFER_D(gBufferDepthIDUV, 3)
        aoDescriptor = std::make_unique<DescriptorInstance>();
        aoDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
        aoDescriptor->create(vulkanContext);
        aoDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                         context.coreFrameBuffers.aoFBO->attachments[0]->vkImageView);
        aoDescriptor->write(vulkanContext);

        // BRDF TEXTURE
        {
            brdfDescriptor = std::make_unique<DescriptorInstance>();
            brdfDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                             0);
            brdfDescriptor->create(vulkanContext);
            brdfDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                               context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                               context.coreTextures.brdf->vkImageView);
            brdfDescriptor->write(vulkanContext);
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
                                                         context.coreFrameBuffers.auxFBO->vkImageSampler,
                                                         context.coreFrameBuffers.auxFBO->attachments[0]->
                                                         vkImageView);
            postProcessingDescriptor->write(vulkanContext);
        }
    }
} // Metal
