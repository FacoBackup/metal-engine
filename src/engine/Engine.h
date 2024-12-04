#ifndef METAL_ENGINE_ENGINE_H
#define METAL_ENGINE_ENGINE_H

#include "../common/Initializable.h"
#include "../common/Synchornizable.h"
#include "system/camera/CameraSystem.h"

namespace Metal {
    class Engine final : AbstractSystem, Initializable {
    public:
        explicit Engine(ApplicationContext &context);

        CameraSystem cameraSystem{context};
        CameraRepository cameraRepository{context};

        void onInitialize() override;

        void onSync() override;
    };
}
#endif
