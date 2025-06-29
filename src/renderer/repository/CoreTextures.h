#ifndef CORETEXTURES_H
#define CORETEXTURES_H
#include "../../common/AbstractRuntimeComponent.h"


namespace Metal {
    struct TextureInstance;

    class CoreTextures final : public AbstractRuntimeComponent {
    public:
        void onInitialize() override;

        TextureInstance *giSurfaceCache = nullptr;
        TextureInstance *currentFrame = nullptr;
        TextureInstance *currentPositions = nullptr;
        TextureInstance *previousFrameMetadata = nullptr;
        TextureInstance *previousFrame = nullptr;
    };
} // Metal

#endif //CORETEXTURES_H
