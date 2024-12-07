#ifndef CORESHADERS_H
#define CORESHADERS_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct CoreShaders final : AbstractCoreRepository {
        explicit CoreShaders(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        void dispose() override;
    };
} // Metal

#endif //CORESHADERS_H
