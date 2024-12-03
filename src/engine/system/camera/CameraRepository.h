#ifndef CAMERAREPOSITORY_H
#define CAMERAREPOSITORY_H
#include "Camera.h"
#include "Frustum.h"
#include "../../../common/Inspectable.h"
#include "glm/mat4x4.hpp"

namespace Metal {
    struct CameraRepository final : Inspectable {
        float rotationSensitivity = 1;
        float movementSensitivity = 1.0f;
        float zoomSensitivity = 1.0f;
        bool motionBlurEnabled = false;
        float motionBlurVelocityScale = 1.f;
        int motionBlurMaxSamples = 50;
        bool cameraMotionBlur = false;
        bool bloomEnabled = false;
        bool filmGrain = false;
        bool vignetteEnabled = false;
        bool chromaticAberrationEnabled = false;
        bool distortionEnabled = false;
        bool DOF = false;
        int focusDistanceDOF = 10;
        float apertureDOF = 1.2f;
        int focalLengthDOF = 5;
        int samplesDOF = 100;
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
        glm::mat4x4 invProjectionMatrix{};
        glm::mat4x4 viewProjectionMatrix{};
        glm::mat4x4 skyboxProjectionMatrix{};
        glm::mat4x4 invSkyboxProjectionMatrix{};

        Frustum frustum{};
        Camera currentCamera{};

        float lastMouseX = 0;
        float lastMouseY = 0;
        float deltaX = 0;
        float deltaY = 0;

        void registerFields() override {
            registerField([this](const std::any &value) {
                rotationSensitivity = std::any_cast<float>(value);
            }, "Controls", "Camera rotation sensitivity");
            registerField([this](const std::any &value) {
                movementSensitivity = std::any_cast<float>(value);
            }, "Controls", "Camera Movement speed");
            registerField([this](const std::any &value) {
                zoomSensitivity = std::any_cast<float>(value);
            }, "Controls", "Orbit camera zoom sensitivity");
            registerField([this](const std::any &value) {
                motionBlurEnabled = std::any_cast<bool>(value);
            }, "Motion blur", "Motion Blur Enabled");
            registerField([this](const std::any &value) {
                motionBlurVelocityScale = std::any_cast<float>(value);
            }, "Motion blur", "Motion Blur Velocity Scale");
            registerField([this](const std::any &value) {
                motionBlurMaxSamples = std::any_cast<int>(value);
            }, "Motion blur", "Motion Blur Max Samples");
            registerField([this](const std::any &value) {
                cameraMotionBlur = std::any_cast<bool>(value);
            }, "Motion blur", "Camera Motion Blur");
            registerField([this](const std::any &value) {
                bloomEnabled = std::any_cast<bool>(value);
            }, "Post processing", "Bloom");
            registerField([this](const std::any &value) {
                filmGrain = std::any_cast<bool>(value);
            }, "Post processing", "Film Grain");
            registerField([this](const std::any &value) {
                vignetteEnabled = std::any_cast<bool>(value);
            }, "Post processing", "Vignette Enabled");
            registerField([this](const std::any &value) {
                chromaticAberrationEnabled = std::any_cast<bool>(value);
            }, "Post processing", "Chromatic Aberration");
            registerField([this](const std::any &value) {
                distortionEnabled = std::any_cast<bool>(value);
            }, "Post processing", "Distortion");
            registerField([this](const std::any &value) {
                DOF = std::any_cast<bool>(value);
            }, "Depth of Field", "Enable DOF");
            registerField([this](const std::any &value) {
                focusDistanceDOF = std::any_cast<int>(value);
            }, "Depth of Field", "Focus Distance (DOF)");
            registerField([this](const std::any &value) {
                apertureDOF = std::any_cast<float>(value);
            }, "Depth of Field", "Aperture (DOF)");
            registerField([this](const std::any &value) {
                focalLengthDOF = std::any_cast<int>(value);
            }, "Depth of Field", "Focal Length (DOF)");
            registerField([this](const std::any &value) {
                samplesDOF = std::any_cast<int>(value);
            }, "Depth of Field", "DOF Samples");
            registerField([this](const std::any &value) {
                filmGrainStrength = std::any_cast<float>(value);
            }, "Post processing", "Film Grain Strength");
            registerField([this](const std::any &value) {
                vignetteStrength = std::any_cast<float>(value);
            }, "Post processing", "Vignette Strength");
            registerField([this](const std::any &value) {
                bloomThreshold = std::any_cast<float>(value);
            }, "Post processing", "Bloom Threshold");
            registerField([this](const std::any &value) {
                bloomQuality = std::any_cast<int>(value);
            }, "Post processing", "Bloom Quality");
            registerField([this](const std::any &value) {
                bloomOffset = std::any_cast<int>(value);
            }, "Post processing", "Bloom Offset");
            registerField([this](const std::any &value) {
                chromaticAberrationIntensity = std::any_cast<float>(value);
            }, "Post processing", "Chromatic Aberration Strength", 0, MAX_INT, false);
            registerField([this](const std::any &value) {
                distortionIntensity = std::any_cast<float>(value);
            }, "Post processing", "Distortion Strength", 0, MAX_INT, false);
        }
    };
}

#endif
