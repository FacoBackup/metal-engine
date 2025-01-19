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
        std::unique_ptr<DescriptorInstance> materialAlbedo = nullptr;
        std::unique_ptr<DescriptorInstance> materialNormal = nullptr;
        std::unique_ptr<DescriptorInstance> materialRoughness = nullptr;
        std::unique_ptr<DescriptorInstance> materialMetallic = nullptr;
        std::unique_ptr<DescriptorInstance> materialAO = nullptr;
        std::unique_ptr<DescriptorInstance> materialHeight = nullptr;
        std::unique_ptr<DescriptorInstance> postProcessingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> voxelPositionDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> voxelHitPositionDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheFragment = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferAlbedo = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferNormal = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferPosition = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheCompute = nullptr;
        std::unique_ptr<DescriptorInstance> giCompute = nullptr;
        std::unique_ptr<DescriptorInstance> shadingCompute = nullptr;
        std::unique_ptr<DescriptorInstance> svoData = nullptr;
        std::unique_ptr<DescriptorInstance> lightsData = nullptr;

        void onInitialize() override;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
