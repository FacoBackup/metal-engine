#ifndef DESCRIPTORPOOLSERVICE_H
#define DESCRIPTORPOOLSERVICE_H

#include <vulkan/vulkan_core.h>
#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct TextureInstance;
    struct FrameBufferAttachment;
    struct FrameBufferInstance;
    struct DescriptorInstance;

    class DescriptorService final : public AbstractRuntimeComponent {
    public:
        explicit DescriptorService(ApplicationContext &context);

        void setImageDescriptor(const FrameBufferInstance *framebuffer, unsigned int attachmentIndex) const;

        void setImageDescriptor(TextureInstance *texture) const;
    };
} // Metal

#endif //DESCRIPTORPOOLSERVICE_H
