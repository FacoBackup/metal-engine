#ifndef CAMERAREPOSITORY_H
#define CAMERAREPOSITORY_H
#include "Camera.h"
#include "Frustum.h"
#include "glm/mat4x4.hpp"

namespace Metal {
    struct CameraRepository {
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
    };
}

#endif
