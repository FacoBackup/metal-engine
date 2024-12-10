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

        DescriptorInstance *debugDescriptor = nullptr;
        DescriptorInstance *currentFrameImageDescriptor = nullptr;

        void onInitialize() override;

        void writeBindings() const;

        void createDescriptors() const;
    };
} // Metal

#endif //COREDESCRIPTORSETS_H
