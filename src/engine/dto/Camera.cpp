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
        registerEditableField<FLOAT>(&fov).setName("FOV").setGroup("View").setMin(60).setMax(120);
        registerEditableField<FLOAT>(&zNear).setName("Z-Near").setGroup("View").setMin(0).setMax(1);
        registerEditableField<FLOAT>(&zFar).setName("Z-Far").setGroup("View").setMin(1).setMax(MAX_INT);

        registerEditableField<FLOAT>(&rotationSensitivity).setName("Camera rotation sensitivity").setGroup("Controls").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&movementSensitivity).setName("Camera Movement speed").setGroup("Controls").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&zoomSensitivity).setName("Orbit camera zoom sensitivity").setGroup("Controls").setMin(0).setMax(10);

        registerEditableField<BOOLEAN>(&motionBlurEnabled).setName("Motion Blur Enabled").setGroup("Motion blur");
        registerEditableField<FLOAT>(&motionBlurVelocityScale).setName("Motion Blur Velocity Scale").setGroup("Motion blur").setMin(0).setMax(500);
        registerEditableField<INT>(&motionBlurMaxSamples).setName("Motion Blur Max Samples").setGroup("Motion blur").setMin(1).setMax(100);
        registerEditableField<BOOLEAN>(&cameraMotionBlur).setName("Camera Motion Blur").setGroup("Motion blur");

        registerEditableField<BOOLEAN>(&bloomEnabled).setName("Bloom").setGroup("Post processing");
        registerEditableField<BOOLEAN>(&filmGrain).setName("Film Grain").setGroup("Post processing");
        registerEditableField<BOOLEAN>(&vignetteEnabled).setName("Vignette Enabled").setGroup("Post processing");
        registerEditableField<BOOLEAN>(&chromaticAberrationEnabled).setName("Chromatic Aberration").setGroup("Post processing");
        registerEditableField<BOOLEAN>(&distortionEnabled).setName("Distortion").setGroup("Post processing");
        registerEditableField<FLOAT>(&filmGrainStrength).setName("Film Grain Strength").setGroup("Post processing").setMin(-MAX_FLOAT).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&vignetteStrength).setName("Vignette Strength").setGroup("Post processing").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&bloomThreshold).setName("Bloom Threshold").setGroup("Post processing").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<INT>(&bloomQuality).setName("Bloom Quality").setGroup("Post processing").setMin(0).setMax(MAX_INT);
        registerEditableField<INT>(&bloomOffset).setName("Bloom Offset").setGroup("Post processing").setMin(0).setMax(MAX_INT);
        registerEditableField<FLOAT>(&chromaticAberrationIntensity).setName("Chromatic Aberration Strength").setGroup("Post processing").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&distortionIntensity).setName("Distortion Strength").setGroup("Post processing").setMin(0).setMax(MAX_FLOAT);

        registerSerializableOnlyField<VECTOR3>(&position).setName("position");
        registerSerializableOnlyField<BOOLEAN>(&isOrthographic).setName("isOrthographic");
        registerSerializableOnlyField<FLOAT>(&aspectRatio).setName("aspectRatio");
        registerSerializableOnlyField<FLOAT>(&orthographicProjectionSize).setName("orthographicProjectionSize");
        registerSerializableOnlyField<FLOAT>(&pitch).setName("pitch");
        registerSerializableOnlyField<FLOAT>(&yaw).setName("yaw");
        registerSerializableOnlyField<FLOAT>(&lastMouseX).setName("lastMouseX");
        registerSerializableOnlyField<FLOAT>(&lastMouseY).setName("lastMouseY");
        registerSerializableOnlyField<FLOAT>(&deltaX).setName("deltaX");
        registerSerializableOnlyField<FLOAT>(&deltaY).setName("deltaY");
    }

    Camera::Camera(const float pitch, const float yaw, const glm::vec3 position) {
        this->pitch = pitch;
        this->yaw = yaw;
        this->position = position;
    }
}
