#ifndef CORETEXTURES_H
#define CORETEXTURES_H
#include "../abstract/AbstractCoreRepository.h"


namespace Metal {
    struct TextureInstance;

    class CoreTextures final : public AbstractCoreRepository {
    public:
        explicit CoreTextures(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        TextureInstance *giSurfaceCache = nullptr;
        TextureInstance *currentFrame = nullptr;
        TextureInstance *previousFrameMetadata = nullptr;
        TextureInstance *previousFrame = nullptr;
    };
} // Metal

#endif //CORETEXTURES_H
