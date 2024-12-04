
#include "CameraRepository.h"

namespace Metal {

    void Metal::CameraRepository::registerFields() {
        registerFloat(rotationSensitivity, "Controls", "Camera rotation sensitivity", 0, 10, false);

        registerFloat(movementSensitivity, "Controls", "Camera Movement speed", 0, 10, false);

        registerFloat(zoomSensitivity, "Controls", "Orbit camera zoom sensitivity", 0, 10, false);

        registerBool(motionBlurEnabled,
                     "Motion blur", "Motion Blur Enabled", false);

        registerFloat(motionBlurVelocityScale,
                      "Motion blur", "Motion Blur Velocity Scale", 0, 500, false);

        registerInt(motionBlurMaxSamples,
                    "Motion blur", "Motion Blur Max Samples", 1, 100, false);

        registerBool(cameraMotionBlur,
                     "Motion blur", "Camera Motion Blur", false);

        registerBool(bloomEnabled,
                     "Post processing", "Bloom", false);

        registerBool(filmGrain,
                     "Post processing", "Film Grain", false);

        registerBool(vignetteEnabled,
                     "Post processing", "Vignette Enabled", false);

        registerBool(chromaticAberrationEnabled,
                     "Post processing", "Chromatic Aberration", false);

        registerBool(distortionEnabled,
                     "Post processing", "Distortion", false);

        registerFloat(filmGrainStrength,
                      "Post processing", "Film Grain Strength", -MAX_FLOAT, MAX_FLOAT, false);

        registerFloat(vignetteStrength,
                      "Post processing", "Vignette Strength", 0, MAX_FLOAT, false);

        registerFloat(bloomThreshold,
                      "Post processing", "Bloom Threshold", 0, MAX_FLOAT, false);

        registerInt(bloomQuality,
                    "Post processing", "Bloom Quality", 0, MAX_INT, false);

        registerInt(bloomOffset,
                    "Post processing",
                    "Bloom Offset", 0, MAX_INT, false);

        registerFloat(chromaticAberrationIntensity, "Post processing", "Chromatic Aberration Strength", 0,
                      MAX_FLOAT, false);

        registerFloat(distortionIntensity, "Post processing", "Distortion Strength", 0, MAX_FLOAT, false);

    }

    CameraRepository::CameraRepository(ApplicationContext &context) : AbstractRepository(context) {}
}