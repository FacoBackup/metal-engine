#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "CameraRepository.h"
#include "../../../common/Synchornizable.h"

namespace Metal {
    class CameraSystem final : public Syncronizable {
    public:
        void onSync() override;

        CameraRepository cameraRepository;
    };
}

#endif
