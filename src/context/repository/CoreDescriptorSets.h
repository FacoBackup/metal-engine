#ifndef COREDESCRIPTORSETS_H
#define COREDESCRIPTORSETS_H

#include <memory>

#include "AbstractCoreRepository.h"
#include "../runtime/DescriptorInstance.h"

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
        std::unique_ptr<DescriptorInstance> brdfDescriptor = nullptr;

        void onInitialize() override;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
