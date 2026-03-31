#ifndef EDITORCAMERASERVICE_H
#define EDITORCAMERASERVICE_H

#include <common/IService.h>
#include <common/IEventMember.h>
#include <common/IInit.h>
#include <glm/glm.hpp>

namespace Metal {
    struct CameraRepository;
    class EngineContext;
    struct RuntimeRepository;
    class DirtyStateService;

    class EditorCameraService final : public IService, public IEventMember, public IInit {
        EngineContext *engineContext = nullptr;
        RuntimeRepository *runtimeRepository = nullptr;
        DirtyStateService *dirtyStateService = nullptr;
        CameraRepository *cameraRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EngineContext", &engineContext},
                {"RuntimeRepository", &runtimeRepository},
                {"CameraRepository", &cameraRepository},
                {"DirtyStateService", &dirtyStateService}
            };
        }


        void onInitialize() override;

    private:
        void move(const glm::vec3 &direction);

        void rotate(float deltaX, float deltaY);
    };
}

#endif //EDITORCAMERASERVICE_H
