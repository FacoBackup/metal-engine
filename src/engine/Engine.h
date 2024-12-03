#ifndef METAL_ENGINE_ENGINE_H
#define METAL_ENGINE_ENGINE_H

#include "../common/Initializable.h"
#include "../common/Synchornizable.h"
#include "system/camera/CameraSystem.h"

namespace Metal {
    class Engine final : public Syncronizable, Initializable {
    private:
        CameraSystem cameraSystem;
    public:
        void onInitialize() override;

        void onSync() override;
    };
}
#endif
