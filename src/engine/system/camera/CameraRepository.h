#ifndef CAMERAREPOSITORY_H
#define CAMERAREPOSITORY_H

#include "Camera.h"
#include "Frustum.h"
#include "../../../common/inspection/Inspectable.h"
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
            registerField(FLOAT,
                          GETTER(rotationSensitivity),
                          SETTER(rotationSensitivity, float),
                          "Controls", "Camera rotation sensitivity");

            registerField(FLOAT,
                          GETTER(movementSensitivity),
                          SETTER(movementSensitivity, float),
                          "Controls", "Camera Movement speed");

            registerField(FLOAT,
                          GETTER(zoomSensitivity),
                          SETTER(zoomSensitivity, float),
                          "Controls", "Orbit camera zoom sensitivity");

            registerField(BOOLEAN,
                          GETTER(motionBlurEnabled),
                          SETTER(motionBlurEnabled, bool),
                          "Motion blur", "Motion Blur Enabled");

            registerField(FLOAT,
                          GETTER(motionBlurVelocityScale),
                          SETTER(motionBlurVelocityScale, float),
                          "Motion blur", "Motion Blur Velocity Scale");

            registerField(INT,
                          GETTER(motionBlurMaxSamples),
                          SETTER(motionBlurMaxSamples, int),
                          "Motion blur", "Motion Blur Max Samples");

            registerField(BOOLEAN,
                          GETTER(cameraMotionBlur),
                          SETTER(cameraMotionBlur, bool),
                          "Motion blur", "Camera Motion Blur");

            registerField(BOOLEAN,
                          GETTER(bloomEnabled),
                          SETTER(bloomEnabled, bool),
                          "Post processing", "Bloom");

            registerField(BOOLEAN,
                          GETTER(filmGrain),
                          SETTER(filmGrain, bool),
                          "Post processing", "Film Grain");

            registerField(BOOLEAN,
                          GETTER(vignetteEnabled),
                          SETTER(vignetteEnabled, bool),
                          "Post processing", "Vignette Enabled");

            registerField(BOOLEAN,
                          GETTER(chromaticAberrationEnabled),
                          SETTER(chromaticAberrationEnabled, bool),
                          "Post processing", "Chromatic Aberration");

            registerField(BOOLEAN,
                          GETTER(distortionEnabled),
                          SETTER(distortionEnabled, bool),
                          "Post processing", "Distortion");

            registerField(FLOAT,
                          GETTER(filmGrainStrength),
                          SETTER(filmGrainStrength, float),
                          "Post processing", "Film Grain Strength");

            registerField(FLOAT,
                          GETTER(vignetteStrength),
                          SETTER(vignetteStrength, float),
                          "Post processing", "Vignette Strength");

            registerField(FLOAT,
                          GETTER(bloomThreshold),
                          SETTER(bloomThreshold, float),
                          "Post processing", "Bloom Threshold");

            registerField(INT,
                          GETTER(bloomQuality),
                          SETTER(bloomQuality, int),
                          "Post processing", "Bloom Quality");

            registerField(INT,
                          GETTER(bloomOffset),
                          SETTER(bloomOffset, int),
                          "Post processing",
                          "Bloom Offset");

            registerField(FLOAT,
                          GETTER(chromaticAberrationIntensity),
                          SETTER(chromaticAberrationIntensity, float),
                          "Post processing", "Chromatic Aberration Strength", 0, MAX_INT, false);

            registerField(FLOAT,
                          GETTER(distortionIntensity),
                          SETTER(distortionIntensity, float),
                          "Post processing", "Distortion Strength", 0, MAX_INT, false);
        }
    };
}

#endif
