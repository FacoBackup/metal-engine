#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "../../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class ApplicationContext;
    class Camera;

    class CameraSystem final : public AbstractRuntimeComponent {
        Camera *camera = nullptr;

        void updateMatrices() const;

        void updateView() const;

        void updateProjection() const;

        void updateAspectRatio() const;

    public:
        explicit CameraSystem(ApplicationContext &context);

        void onSync() override;
    };
}

#endif
