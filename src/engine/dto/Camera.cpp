#include "Camera.h"

#include "../../common/Icons.h"

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

    const char *Camera::getTitle() const {
        return "Camera";
    }

    const char *Camera::getIcon() const {
        return Icons::camera_alt.c_str();
    }

    void Camera::registerFields() {
        registerEditableField(&fov, FLOAT, "FOV", "View").setMin(60).setMax(120);
        registerEditableField(&zNear, FLOAT, "Z-Near", "View").setMin(0).setMax(1);
        registerEditableField(&zFar, FLOAT, "Z-Far", "View").setMin(1).setMax(MAX_INT);

        registerEditableField(&rotationSensitivity, FLOAT, "Camera rotation sensitivity", "Controls").setMin(0).setMax(10);
        registerEditableField(&movementSensitivity, FLOAT, "Camera Movement speed", "Controls").setMin(0).setMax(10);
        registerEditableField(&zoomSensitivity, FLOAT, "Orbit camera zoom sensitivity", "Controls").setMin(0).setMax(10);

        registerEditableField(&motionBlurEnabled, BOOLEAN, "Motion Blur Enabled", "Motion blur");
        registerEditableField(&motionBlurVelocityScale, FLOAT, "Motion Blur Velocity Scale", "Motion blur").setMin(0).setMax(500);
        registerEditableField(&motionBlurMaxSamples, INT, "Motion Blur Max Samples", "Motion blur").setMin(1).setMax(100);
        registerEditableField(&cameraMotionBlur, BOOLEAN, "Camera Motion Blur", "Motion blur");

        registerEditableField(&bloomEnabled, BOOLEAN, "Bloom", "Post processing");
        registerEditableField(&filmGrain, BOOLEAN, "Film Grain", "Post processing");
        registerEditableField(&vignetteEnabled, BOOLEAN, "Vignette Enabled", "Post processing");
        registerEditableField(&chromaticAberrationEnabled, BOOLEAN, "Chromatic Aberration", "Post processing");
        registerEditableField(&distortionEnabled, BOOLEAN, "Distortion", "Post processing");
        registerEditableField(&filmGrainStrength, FLOAT, "Film Grain Strength", "Post processing").setMin(-MAX_FLOAT).setMax(MAX_FLOAT);
        registerEditableField(&vignetteStrength, FLOAT, "Vignette Strength", "Post processing").setMin(0).setMax(MAX_FLOAT);
        registerEditableField(&bloomThreshold, FLOAT, "Bloom Threshold", "Post processing").setMin(0).setMax(MAX_FLOAT);
        registerEditableField(&bloomQuality, INT, "Bloom Quality", "Post processing").setMin(0).setMax(MAX_INT);
        registerEditableField(&bloomOffset, INT, "Bloom Offset", "Post processing").setMin(0).setMax(MAX_INT);
        registerEditableField(&chromaticAberrationIntensity, FLOAT, "Chromatic Aberration Strength", "Post processing").setMin(0).setMax(MAX_FLOAT);
        registerEditableField(&distortionIntensity, FLOAT, "Distortion Strength", "Post processing").setMin(0).setMax(MAX_FLOAT);

        registerSerializableOnlyField(&position, VECTOR3, "position");
        registerSerializableOnlyField(&isOrthographic, BOOLEAN, "isOrthographic");
        registerSerializableOnlyField(&aspectRatio, FLOAT, "aspectRatio");
        registerSerializableOnlyField(&orthographicProjectionSize, FLOAT, "orthographicProjectionSize");
        registerSerializableOnlyField(&pitch, FLOAT, "pitch");
        registerSerializableOnlyField(&yaw, FLOAT, "yaw");
        registerSerializableOnlyField(&lastMouseX, FLOAT, "lastMouseX");
        registerSerializableOnlyField(&lastMouseY, FLOAT, "lastMouseY");
        registerSerializableOnlyField(&deltaX, FLOAT, "deltaX");
        registerSerializableOnlyField(&deltaY, FLOAT, "deltaY");
    }

    Camera::Camera(const float pitch, const float yaw, const glm::vec3 position) {
        this->pitch = pitch;
        this->yaw = yaw;
        this->position = position;
    }
}
