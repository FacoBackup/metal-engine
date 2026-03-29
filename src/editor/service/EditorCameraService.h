#ifndef EDITORCAMERASERVICE_H
#define EDITORCAMERASERVICE_H

#include <common/IService.h>
#include <common/IEventMember.h>
#include <common/IInit.h>
#include <glm/glm.hpp>

namespace Metal {
    struct Camera;
    struct EditorCameraRepository;
    class EngineContext;
    struct RuntimeRepository;
    class DirtyStateService;

    class EditorCameraService final : public IService, public IEventMember, public IInit {
        EngineContext *engineContext = nullptr;
        EditorCameraRepository *editorCameraRepository = nullptr;
        RuntimeRepository *runtimeRepository = nullptr;
        DirtyStateService *dirtyStateService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override;

        void onInitialize() override;

    private:
        void move(const glm::vec3 &direction);

        void rotate(float deltaX, float deltaY);
    };
}

#endif //EDITORCAMERASERVICE_H
