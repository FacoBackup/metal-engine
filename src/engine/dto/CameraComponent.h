#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "AbstractComponent.h"
#include <glm/glm.hpp>

namespace Metal {
    struct CameraComponent final : AbstractComponent {
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
        float yaw = 0;
        float pitch = 0;
        bool mainCamera = false;

        bool isOrthographic = false;
        float aspectRatio = 1;
        float orthographicProjectionSize = 50;

        void registerFields() override;

        ComponentType getType() const override;
    };
}

#endif // CAMERACOMPONENT_H
