#include "DescriptorService.h"

#include "DescriptorBinding.h"
#include "DescriptorInstance.h"
#include "../../context/ApplicationContext.h"

#include "../framebuffer/FrameBufferAttachment.h"
#include "../framebuffer/FrameBufferInstance.h"
#include "../texture/TextureInstance.h"

namespace Metal {
    DescriptorService::DescriptorService(ApplicationContext &context)
        : AbstractRuntimeComponent(context) {
    }

    void DescriptorService::setImageDescriptor(const FrameBufferInstance *framebuffer,
                                               unsigned int attachmentIndex) const {
        auto attachment = framebuffer->attachments[attachmentIndex];
        if (attachment->imageDescriptor == nullptr) {
            attachment->imageDescriptor = std::make_unique<DescriptorInstance>();
            attachment->imageDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                framebuffer->vkImageSampler,
                attachment->vkImageView));
            attachment->imageDescriptor->create(context.vulkanContext);
        }
    }

    void DescriptorService::setImageDescriptor(TextureInstance *texture) const {
        if (texture->imageDescriptor == nullptr) {
            texture->imageDescriptor = std::make_unique<DescriptorInstance>();
            texture->imageDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             texture->vkSampler,
                                                                             texture->vkImageView
            ));
            texture->imageDescriptor->create(context.vulkanContext);
        }
    }
} // Metal
