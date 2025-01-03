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
        std::unique_ptr<DescriptorInstance> postProcessingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferShadingDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferShadingDescriptor1 = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferShadingDescriptor2 = nullptr;
        std::unique_ptr<DescriptorInstance> gBufferShadingDescriptor3 = nullptr;
        std::unique_ptr<DescriptorInstance> aoDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> svoData = nullptr;
        std::unique_ptr<DescriptorInstance> lightsData = nullptr;
        std::unique_ptr<DescriptorInstance> brdfDescriptor = nullptr;
        std::unique_ptr<DescriptorInstance> iconsDescriptor = nullptr;

        void onInitialize() override;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
