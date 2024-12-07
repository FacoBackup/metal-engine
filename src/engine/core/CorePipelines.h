#ifndef COREPIPELINES_H
#define COREPIPELINES_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct CorePipelines final : AbstractCoreRepository {
        explicit CorePipelines(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;
    };
} // Metal

#endif //CORESHADERS_H
