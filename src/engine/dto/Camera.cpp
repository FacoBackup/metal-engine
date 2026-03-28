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
        registerEditableField<FLOAT>(&fov).setName("fov").setLabel("Field of view").setMin(60).setMax(120);
        registerEditableField<FLOAT>(&zNear).setName("zNear").setLabel("Near plane").setMin(0).setMax(1);
        registerEditableField<FLOAT>(&zFar).setName("zFar").setLabel("Far plane").setMin(1).setMax(MAX_INT);

        registerEditableField<FLOAT>(&rotationSensitivity).setName("rotationSensitivity").setLabel("Rotation sensitivity").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&movementSensitivity).setName("movementSensitivity").setLabel("Movement sensitivity").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&zoomSensitivity).setName("zoomSensitivity").setLabel("Zoom sensitivity").setMin(0).setMax(10);

        registerEditableField<BOOLEAN>(&motionBlurEnabled).setName("motionBlurEnabled").setLabel("Motion blur enabled");
        registerEditableField<FLOAT>(&motionBlurVelocityScale).setName("motionBlurVelocityScale").setLabel("Motion blur velocity scale").setMin(0).setMax(500);
        registerEditableField<INT>(&motionBlurMaxSamples).setName("motionBlurMaxSamples").setLabel("Motion blur max samples").setMin(1).setMax(100);
        registerEditableField<BOOLEAN>(&cameraMotionBlur).setName("cameraMotionBlur").setLabel("Camera motion blur enabled");

        registerEditableField<BOOLEAN>(&bloomEnabled).setName("bloomEnabled").setLabel("Bloom enabled");
        registerEditableField<BOOLEAN>(&filmGrain).setName("filmGrain").setLabel("Film grain enabled");
        registerEditableField<BOOLEAN>(&vignetteEnabled).setName("vignetteEnabled").setLabel("Vignette enabled");
        registerEditableField<BOOLEAN>(&chromaticAberrationEnabled).setName("chromaticAberrationEnabled").setLabel("Chromatic aberration enabled");
        registerEditableField<BOOLEAN>(&distortionEnabled).setName("distortionEnabled").setLabel("Distortion enabled");
        registerEditableField<FLOAT>(&filmGrainStrength).setName("filmGrainStrength").setLabel("Film grain strength").setMin(-MAX_FLOAT).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&vignetteStrength).setName("vignetteStrength").setLabel("Vignette strength").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&bloomThreshold).setName("bloomThreshold").setLabel("Bloom threshold").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<INT>(&bloomQuality).setName("bloomQuality").setLabel("Bloom quality").setMin(0).setMax(MAX_INT);
        registerEditableField<INT>(&bloomOffset).setName("bloomOffset").setLabel("Bloom offset").setMin(0).setMax(MAX_INT);
        registerEditableField<FLOAT>(&chromaticAberrationIntensity).setName("chromaticAberrationIntensity").setLabel("Chromatic aberration intensity").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&distortionIntensity).setName("distortionIntensity").setLabel("Distortion intensity").setMin(0).setMax(MAX_FLOAT);

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
