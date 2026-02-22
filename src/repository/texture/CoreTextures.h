#ifndef CORETEXTURES_H
#define CORETEXTURES_H
#include "../abstract/AbstractCoreRepository.h"


namespace Metal {
    struct TextureInstance;

    class CoreTextures final : public AbstractCoreRepository {
    public:
        explicit CoreTextures()
            : AbstractCoreRepository() {
        }

        void onInitialize() override;

        TextureInstance *surfaceCache = nullptr;
        TextureInstance *rawRenderedFrame = nullptr;
        TextureInstance *accumulatedFrame = nullptr;
    };
} // Metal

#endif //CORETEXTURES_H
