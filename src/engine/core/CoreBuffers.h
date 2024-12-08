#ifndef COREBUFFERS_H
#define COREBUFFERS_H
#include "AbstractCoreRepository.h"

namespace Metal {

struct CoreBuffers final : AbstractCoreRepository{
    explicit CoreBuffers(ApplicationContext &context)
        : AbstractCoreRepository(context) {
    }

    void onInitialize() override;
};

} // Metal

#endif //COREBUFFERS_H
