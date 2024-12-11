#ifndef COREDESCRIPTORSETS_H
#define COREDESCRIPTORSETS_H

#include "AbstractCoreRepository.h"

namespace Metal {
    struct DescriptorInstance;
}

namespace Metal {
    struct CoreDescriptorSets final : AbstractCoreRepository {
        explicit CoreDescriptorSets(ApplicationContext &context)
                : AbstractCoreRepository(context) {
        }

        DescriptorInstance *globalDataDescriptor = nullptr;
        DescriptorInstance *currentFrameImageDescriptor = nullptr;

        void onInitialize() override;

        void createDescriptors() const;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
