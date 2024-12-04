#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "../../repository/camera/CameraRepository.h"
#include "../../../common/Synchornizable.h"
#include "../../../common/runtime/AbstractSystem.h"

namespace Metal {
    class CameraSystem final : public AbstractSystem {
    public:
        explicit CameraSystem(ApplicationContext &context);

        void onSync() override;
    };
}

#endif
