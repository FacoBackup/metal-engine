#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "../../common/ISync.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../common/IService.h"
#include "../../common/IEventMember.h"
#include "../../common/IInit.h"

constexpr float PI_OVER_2 = glm::pi<float>() / 2.0f;
constexpr float PI_2 = glm::pi<float>() * 2.0f;
constexpr float MIN_MAX_PITCH = glm::radians(89.0f);

namespace Metal {
    class ApplicationContext;
    struct Camera;

    class EngineContext;
    struct WorldRepository;
    struct RuntimeRepository;
    class DirtyStateService;
    struct Camera;

    class CameraService final : public IService, public ISync, public IEventMember, public IInit {
        EngineContext *engineContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        RuntimeRepository *runtimeRepository = nullptr;
        DirtyStateService *dirtyStateService = nullptr;

        Camera *camera = nullptr;
        glm::vec3 xAxis{0.0f};
        glm::vec3 yAxis{0.0f};
        glm::vec3 zAxis{0.0f};

        void updateMatrices();

        void updateView();

        void updateProjection() const;

        void updateAspectRatio() const;

        void createViewMatrix();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EngineContext", &engineContext},
                {"WorldRepository", &worldRepository},
                {"RuntimeRepository", &runtimeRepository},
                {"DirtyStateService", &dirtyStateService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif
