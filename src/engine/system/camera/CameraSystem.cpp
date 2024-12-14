#include "CameraSystem.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void CameraSystem::onSync() {
        const auto &repository = context.getEngineContext().cameraRepository;
        camera = repository.currentCamera;
        if (camera != nullptr) {
            updateAspectRatio();
            if (camera->isNotFrozen()) {
                updateMatrices();
                context.getEngineContext().globalDataNeedsUpdate = true;
                camera->freezeVersion();
            }
        }
    }

    void CameraSystem::updateAspectRatio() const {
        const auto &runtimeRepository = context.getEngineContext().runtimeRepository;

        const float prevAspect = camera->aspectRatio;
        camera->aspectRatio = runtimeRepository.viewportW / runtimeRepository.viewportH;
        if (prevAspect != camera->aspectRatio) {
            camera->registerChange();
        }
    }

    void CameraSystem::updateMatrices() const {
        auto &repository = context.getEngineContext().cameraRepository;

        updateProjection();
        updateView();
        repository.projViewMatrix = repository.projectionMatrix * repository.viewMatrix;
        repository.frustum.extractFrustumPlanes(repository.projViewMatrix);
    }

    void CameraSystem::updateView() const {
        auto &repository = context.getEngineContext().cameraRepository;

        context.getEngineContext().cameraMovementService.createViewMatrix(*camera);
        repository.invViewMatrix = inverse(repository.viewMatrix);
    }

    void CameraSystem::updateProjection() const {
        auto &runtimeRepository = context.getEngineContext().runtimeRepository;
        auto &repository = context.getEngineContext().cameraRepository;

        camera->aspectRatio = runtimeRepository.viewportW / runtimeRepository.viewportH;
        if (camera->isOrthographic) {
            repository.projectionMatrix = glm::ortho(-camera->orthographicProjectionSize,
                                                     camera->orthographicProjectionSize,
                                                     -camera->orthographicProjectionSize / camera->aspectRatio,
                                                     camera->orthographicProjectionSize / camera->aspectRatio,
                                                     -camera->zFar, camera->zFar);
        } else {
            repository.projectionMatrix = glm::perspective(camera->fov, camera->aspectRatio, camera->zNear,
                                                           camera->zFar);
        }
        repository.projectionMatrix = glm::transpose(repository.projectionMatrix);
        repository.invProjectionMatrix = inverse(repository.projectionMatrix);
    }

    CameraSystem::CameraSystem(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }
}
