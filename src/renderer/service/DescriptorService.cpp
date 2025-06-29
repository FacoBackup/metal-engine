#include "DescriptorService.h"

#include "../data/DescriptorBinding.h"
#include "../data/DescriptorInstance.h"
#include "../../MetalContextProvider.h"

#include "../data/FrameBufferAttachment.h"
#include "../data/FrameBufferInstance.h"
#include "../data/TextureInstance.h"

namespace Metal {

    void DescriptorService::setImageDescriptor(const FrameBufferInstance *framebuffer,
                                               unsigned int attachmentIndex) const {
        auto attachment = framebuffer->attachments[attachmentIndex];
        if (attachment->imageDescriptor == nullptr) {
            attachment->imageDescriptor = std::make_unique<DescriptorInstance>();
            attachment->imageDescriptor->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                SINGLETONS.coreDescriptorSets.vkImageSampler,
                attachment->vkImageView));
            attachment->imageDescriptor->create();
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
            texture->imageDescriptor->create();
        }
    }
} // Metal
