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
        std::unique_ptr<DescriptorInstance> gBufferMaterialDataAlbedo = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialDataNormal = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialDataRoughness = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialDataMetallic = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialDataAO = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialDataHeight = nullptr;
        std::unique_ptr<DescriptorInstance> postProcessingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> voxelPositionDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> voxelHitPositionDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> globalIlluminationDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheFragment = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialA = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialB = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialC = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferMaterialD = nullptr;
        std::unique_ptr<DescriptorInstance> surfaceCacheCompute = nullptr;
        std::unique_ptr<DescriptorInstance> giCompute = nullptr;
        std::unique_ptr<DescriptorInstance> svoData = nullptr;
        std::unique_ptr<DescriptorInstance> lightsData = nullptr;
        std::unique_ptr<DescriptorInstance> iconsDescriptor = nullptr;

        void onInitialize() override;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
