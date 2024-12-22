#ifndef CORETEXTURES_H
#define CORETEXTURES_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct TextureInstance;

    class CoreTextures final : public AbstractCoreRepository {
    public:
        explicit CoreTextures(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        TextureInstance *brdf = nullptr;
    };
} // Metal

#endif //CORETEXTURES_H
