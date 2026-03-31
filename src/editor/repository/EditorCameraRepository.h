#ifndef EDITORCAMERAREPOSITORY_H
#define EDITORCAMERAREPOSITORY_H

#include "../../common/IRepository.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct EditorCameraRepository final : IRepository {
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

        glm::vec3 position{10, 10, 10};
        float pitch = -0.785398f; // -(pi/4)
        float yaw = 0.785398f;   // pi/4

        void clear() override;
        void registerFields() override;
        const char *getIcon() const override;
        const char *getTitle() const override;
    };
}

#endif // EDITORCAMERAREPOSITORY_H
