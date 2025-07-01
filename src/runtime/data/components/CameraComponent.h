#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "AbstractComponent.h"
#include "../../../common/serialization-definitions.h"

namespace Metal {
    struct CameraComponent final : AbstractComponent {
        float fov = 90.0f;
        float zNear = 0.1f;
        float zFar = 1000.0f;
        float rotationSensitivity = 1.0f;
        float movementSensitivity = 1.0f;
        float zoomSensitivity = 1.0f;
        bool motionBlurEnabled = false;
        float motionBlurVelocityScale = 1.0f;
        int motionBlurMaxSamples = 50;
        bool cameraMotionBlur = false;
        bool bloomEnabled = false;
        bool filmGrain = false;
        bool vignetteEnabled = false;
        bool chromaticAberrationEnabled = false;
        bool distortionEnabled = false;
        float filmGrainStrength = 1.0f;
        float vignetteStrength = 0.250f;
        float bloomThreshold = 0.750f;
        int bloomQuality = 1;
        int bloomOffset = 0;
        float chromaticAberrationIntensity = 1.0f;
        float distortionIntensity = 1.0f;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        SERIALIZE_TEMPLATE(
            fov,
            zNear,
            zFar,
            movementSensitivity,
            zoomSensitivity,
            motionBlurEnabled,
            motionBlurVelocityScale,
            motionBlurMaxSamples,
            cameraMotionBlur,
            bloomEnabled,
            filmGrain,
            vignetteEnabled,
            chromaticAberrationEnabled,
            distortionEnabled,
            filmGrainStrength,
            vignetteStrength,
            bloomThreshold,
            bloomQuality,
            bloomOffset,
            chromaticAberrationIntensity,
            distortionIntensity,
            entityId)
    };
}// Metal
#endif //CAMERACOMPONENT_H