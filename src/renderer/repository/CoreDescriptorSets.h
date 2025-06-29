#ifndef COREDESCRIPTORSETS_H
#define COREDESCRIPTORSETS_H

#include <memory>
#include "../../common/AbstractRuntimeComponent.h"

#include "../data/DescriptorInstance.h"

namespace Metal {
    struct TextureInstance;
    struct FrameBufferAttachment;

    struct CoreDescriptorSets final : AbstractRuntimeComponent {
        VkSampler vkImageSampler = VK_NULL_HANDLE;
        std::unique_ptr<DescriptorInstance> globalDataDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> postProcessingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> previousFrameDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> previousFrameMetadataDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> rtDescriptorSet = nullptr;
        std::unique_ptr<DescriptorInstance> lightsBuffer = nullptr;
        std::unique_ptr<DescriptorInstance> volumesBuffer = nullptr;
        std::unique_ptr<DescriptorInstance> currentFrameDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> currentPositionsDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> currentPositionsFragmentDescriptor = nullptr;

        void onInitialize() override;

    private:
        void createBuffersDescriptors();

        void createPathTracingDescriptors();
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
