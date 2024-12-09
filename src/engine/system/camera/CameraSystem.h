#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "../../repository/camera/CameraRepository.h"
#include "../../../common/interface/Synchornizable.h"

namespace Metal {
    class CameraSystem final : public AbstractRuntimeComponent {
    public:
        explicit CameraSystem(ApplicationContext &context);

        void onSync() override;
    };
}

#endif
