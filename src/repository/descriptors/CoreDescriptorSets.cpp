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
            globalDataDescriptor = std::make_unique<DescriptorInstance>();
            globalDataDescriptor->addLayoutBinding(static_cast<VkShaderStageFlagBits>(
                                                       VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT),
                                                   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
            globalDataDescriptor->create(vulkanContext);
            globalDataDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                      context.coreBuffers.globalData);
            globalDataDescriptor->write(vulkanContext);
        }

        GBUFFER_D(gBufferShadingDescriptor, 0)
        GBUFFER_D(gBufferShadingDescriptor1, 1)
        GBUFFER_D(gBufferShadingDescriptor2, 2)
        GBUFFER_D(gBufferShadingDescriptor3, 3) {
            brdfDescriptor = std::make_unique<DescriptorInstance>();
            brdfDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                             0);
            brdfDescriptor->create(vulkanContext);
            brdfDescriptor->addImageDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                               context.coreFrameBuffers.gBufferFBO->vkImageSampler,
                                               context.coreTextures.brdf->vkImageView);
            brdfDescriptor->write(vulkanContext);
        } {
            postProcessingDescriptor = std::make_unique<DescriptorInstance>();
            postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                       0);
            postProcessingDescriptor->addLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
            postProcessingDescriptor->create(vulkanContext);
            postProcessingDescriptor->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                          context.coreBuffers.postProcessingSettings);
            postProcessingDescriptor->addImageDescriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                         context.coreFrameBuffers.auxFBO->vkImageSampler,
                                                         context.coreFrameBuffers.auxFBO->attachments[0]->
                                                         vkImageView);
            postProcessingDescriptor->write(vulkanContext);
        }
    }
} // Metal