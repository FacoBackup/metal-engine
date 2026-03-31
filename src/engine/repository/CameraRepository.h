#ifndef METAL_ENGINE_CAMERAREPOSITORY_H
#define METAL_ENGINE_CAMERAREPOSITORY_H
#include "common/IInit.h"
#include "common/IRepository.h"
#include <glm/glm.hpp>

namespace Metal {
    struct CameraRepository : IRepository, IInit {
        float fov = 90;
        float zNear = .1f;
        float zFar = 10000;

        float rotationSensitivity = 1;
        float movementSensitivity = 1.0f;
        float zoomSensitivity = 1.0f;

        bool motionBlurEnabled = false;
        float motionBlurVelocityScale = 1.f;
        int motionBlurMaxSamples = 50;
        bool cameraMotionBlur = false;

        bool dofEnabled = false;
        float dofFocusDistance = 10;
        float dofAperture = 1.2f;
        float dofFocalLength = 5;

        bool bloomEnabled = false;
        bool filmGrain = false;
        bool vignetteEnabled = false;
        bool chromaticAberrationEnabled = false;
        bool distortionEnabled = false;
        float filmGrainStrength = 1.f;
        float vignetteStrength = .25f;
        float bloomThreshold = .75f;
        int bloomQuality = 8;
        int bloomOffset = 0;
        float chromaticAberrationIntensity = 1;
        float distortionIntensity = 1;

        glm::mat4x4 viewMatrix{};
        glm::mat4x4 projectionMatrix{};
        glm::mat4x4 invViewMatrix{};
        glm::mat4x4 invProjView{};
        glm::mat4x4 invProjectionMatrix{};
        glm::mat4x4 projViewMatrix{};

        glm::vec3 position{};

        bool isOrthographic = false;
        float aspectRatio = 1;
        float orthographicProjectionSize = 50;

        float pitch = 0.f;
        float yaw = 0.f;

        float lastMouseX = 0;
        float lastMouseY = 0;
        float deltaX = 0;
        float deltaY = 0;

        void clear() override {
        }

        const char *getTitle() const override;

        const char *getIcon() const override;

        void extractFrustumPlanes(glm::mat4x4 m);

        bool isSphereInsideFrustum(glm::vec3 center, float radius) const;

        void registerFields() override;

        void onInitialize() override;

    private:
        static void extractPlane(const glm::mat4 &matrix, int index, glm::vec4 &plane);

        glm::vec4 planes[6] = {glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}};
    };
} // Metal

#endif //METAL_ENGINE_CAMERAREPOSITORY_H
