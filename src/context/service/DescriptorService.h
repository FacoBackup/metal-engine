#ifndef DESCRIPTORPOOLSERVICE_H
#define DESCRIPTORPOOLSERVICE_H

#include <vulkan/vulkan_core.h>
#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct TextureInstance;
    struct FrameBufferAttachment;
    struct DescriptorInstance;

    class DescriptorService final : public AbstractRuntimeComponent {
    public:
        explicit DescriptorService(ApplicationContext &context);

        void updateImageSamplerDescriptor(FrameBufferAttachment *attachment) const;

        void updateImageSamplerDescriptor(TextureInstance *texture) const;
    };
} // Metal

#endif //DESCRIPTORPOOLSERVICE_H
