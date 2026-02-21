#ifndef COREDESCRIPTORSETS_H
#define COREDESCRIPTORSETS_H

#include <memory>
#include "../abstract/AbstractCoreRepository.h"

#include "../../service/descriptor/DescriptorInstance.h"

namespace Metal {
    struct TextureInstance;
    struct FrameBufferAttachment;

    struct CoreDescriptorSets final : AbstractCoreRepository {
        explicit CoreDescriptorSets()
            : AbstractCoreRepository() {
        }

        VkSampler vkImageSampler = VK_NULL_HANDLE;
        std::unique_ptr<DescriptorInstance> globalDataDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> postProcessingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheFragment = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferAlbedo = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferNormal = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPosition = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheImage = nullptr;
        std::unique_ptr<DescriptorInstance> previousFrameDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> previousFrameMetadataDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> currentFrameDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> lightData = nullptr;
        std::unique_ptr<DescriptorInstance> volumeData = nullptr;
        std::unique_ptr<DescriptorInstance> materialData = nullptr;
        std::unique_ptr<DescriptorInstance> textureArray = nullptr;
        std::unique_ptr<DescriptorInstance> tlasDescriptor = nullptr;

        void createTextureArrayDescriptor();

        void createBuffersDescriptors();

        void createGBufferDescriptors();

        void createPathTracingDescriptors();

        void onInitialize() override;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
