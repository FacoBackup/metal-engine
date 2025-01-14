#ifndef COREDESCRIPTORSETS_H
#define COREDESCRIPTORSETS_H

#include <memory>
#include "../abstract/AbstractCoreRepository.h"

#include "../../service/descriptor/DescriptorInstance.h"

namespace Metal {
    struct TextureInstance;
    struct FrameBufferAttachment;
    struct CoreDescriptorSets final : AbstractCoreRepository {
        explicit CoreDescriptorSets(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        std::unique_ptr<DescriptorInstance> globalDataDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPositionataAlbedo = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPositionataNormal = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPositionataRoughness = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPositionataMetallic = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPositionataAO = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPositionataHeight = nullptr;
        std::unique_ptr<DescriptorInstance> postProcessingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> voxelPositionDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> voxelHitPositionDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> globalIlluminationDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheFragment = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferAlbedo = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferNormal = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPosition = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheCompute = nullptr;
        std::unique_ptr<DescriptorInstance> giCompute = nullptr;
        std::unique_ptr<DescriptorInstance> svoData = nullptr;
        std::unique_ptr<DescriptorInstance> lightsData = nullptr;
        std::unique_ptr<DescriptorInstance> iconsDescriptor = nullptr;

        void onInitialize() override;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
