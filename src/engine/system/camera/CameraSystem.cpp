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

    void CameraSystem::updateMatrices() {
        auto &repository = context.getEngineContext().cameraRepository;

        updateProjection();
        updateView();
        repository.viewProjectionMatrix = repository.projectionMatrix * repository.viewMatrix;
        repository.frustum.extractFrustumPlanes(repository.viewProjectionMatrix);
    }

    void CameraSystem::updateView() const {
        auto &repository = context.getEngineContext().cameraRepository;

        context.getEngineContext().cameraMovementService.createViewMatrix(*camera);
        repository.viewMatrix = glm::inverse(repository.invViewMatrix);
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
        repository.skyboxProjectionMatrix = glm::perspective(camera->fov, camera->aspectRatio, 0.1f, 1000.f);
        repository.invSkyboxProjectionMatrix = glm::inverse(repository.skyboxProjectionMatrix);
        repository.invProjectionMatrix = glm::inverse(repository.projectionMatrix);
    }

    CameraSystem::CameraSystem(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }
}
