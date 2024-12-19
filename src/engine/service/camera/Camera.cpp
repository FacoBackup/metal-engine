#include "Camera.h"

#include "../../../common/interface/Icons.h"

namespace Metal {
    void Camera::extractFrustumPlanes(glm::mat4x4 m) {
        for (int i = 0; i < 6; i++) {
            extractPlane(m, i, planes[i]);
        }
    }

    bool Camera::isSphereInsideFrustum(const glm::vec3 center, const float radius) const {
        for (const auto plane: planes) {
            if (const float distance = plane.x * center.x + plane.y * center.y + plane.z * center.z + plane.w;
                distance < -radius) {
                return false;
            }
        }
        return true;
    }

    void Camera::extractPlane(const glm::mat4 &matrix, const int index, glm::vec4 &plane) {
        switch (index) {
            case 0: // Left
                plane = matrix[3] + matrix[0];
                break;
            case 1: // Right
                plane = matrix[3] - matrix[0];
                break;
            case 2: // Bottom
                plane = matrix[3] + matrix[1];
                break;
            case 3: // Top
                plane = matrix[3] - matrix[1];
                break;
            case 4: // Near
                plane = matrix[3] + matrix[2];
                break;
            case 5: // Far
                plane = matrix[3] - matrix[2];
                break;
            default: plane = glm::vec4{0, 0, 0, 0};
        }
        // Normalize the plane
        plane /= glm::length(glm::vec3(plane));
    }

    const char *Camera::getTitle() {
        return "Camera";
    }

    const char *Camera::getIcon() {
        return Icons::camera_alt.c_str();
    }

    void Camera::registerFields() {
        registerFloat(fov, "View", "FOV", 60, 120, false);
        registerFloat(zNear, "View", "Z-Near", 0, 1, false);
        registerFloat(zFar, "View", "Z-Far", 1, MAX_INT, false);

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

    Camera::Camera(const float pitch, const float yaw, const glm::vec3 position) {
        this->pitch = pitch;
        this->yaw = yaw;
        this->position = position;
    }
}
