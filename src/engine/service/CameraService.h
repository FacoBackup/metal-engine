#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "../../common/ISync.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../common/IService.h"

constexpr float PI_OVER_2 = glm::pi<float>() / 2.0f;
constexpr float PI_2 = glm::pi<float>() * 2.0f;
constexpr float MIN_MAX_PITCH = glm::radians(89.0f);

namespace Metal {
    class ApplicationContext;
    struct Camera;

    class EngineContext;
    struct WorldRepository;
    struct RuntimeRepository;
    struct Camera;

    class CameraService final : public IService, public ISync {
        EngineContext *engineContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        RuntimeRepository *runtimeRepository = nullptr;

        Camera *camera = nullptr;
        glm::vec3 xAxis{0.0f};
        glm::vec3 yAxis{0.0f};
        glm::vec3 zAxis{0.0f};

        void updateMatrices();

        void updateView();

        void updateProjection() const;

        void updateAspectRatio() const;

        void handleInputInternal() const;

        void handleMouse(bool isFirstMovement) const;

        void updateDelta(bool isFirstMovement) const;

        void createViewMatrix();

    public:
        void handleInput(bool isFirstMovement) const;

        std::vector<Dependency> getDependencies() override {
            return {
                {"EngineContext", engineContext},
                {"WorldRepository", worldRepository},
                {"RuntimeRepository", runtimeRepository}
            };
        }

        void onSync() override;
    };
}

#endif
