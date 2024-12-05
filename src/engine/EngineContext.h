#ifndef METAL_ENGINE_ENGINECONTEXT_H
#define METAL_ENGINE_ENGINECONTEXT_H

#include "../common/Initializable.h"
#include "../common/Synchornizable.h"
#include "system/camera/CameraSystem.h"

namespace Metal {
    class EngineContext final : AbstractRuntimeComponent, Initializable {
    public:
        explicit EngineContext(ApplicationContext &context);

        CameraSystem cameraSystem{context};
        CameraRepository cameraRepository{context};

        void onInitialize() override;

        void onSync() override;
    };
}
#endif
