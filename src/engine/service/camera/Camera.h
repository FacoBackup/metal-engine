#ifndef CAMERA_H
#define CAMERA_H
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "../../../common/inspection/Inspectable.h"
#include "../../../common/util/Util.h"

namespace Metal {
    struct Camera final : Inspectable {
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
        glm::mat4x4 projViewMatrix{};

        glm::vec3 position{};

        bool isOrthographic = false;
        float zFar = 10000;
        float zNear = .1f;
        float fov = 90 * Util::TO_RADIANS;
        float aspectRatio = 1;
        float orthographicProjectionSize = 50;

        void extractFrustumPlanes(glm::mat4x4 m);

        bool isSphereInsideFrustum(glm::vec3 center, float radius) const;

        float pitch = 0.f;
        float yaw = 0.f;

        float lastMouseX = 0;
        float lastMouseY = 0;
        float deltaX = 0;
        float deltaY = 0;

        const char *getTitle() override;

        const char *getIcon() override;

        void registerFields() override;
    private:
        static void extractPlane(const glm::mat4 &matrix, int index, glm::vec4 &plane);

        glm::vec4 planes[6] = {glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}};
    };
}

#endif
