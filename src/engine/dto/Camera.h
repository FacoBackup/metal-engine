#ifndef CAMERA_H
#define CAMERA_H
#define GLM_FORCE_RADIANS
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "../../../common/Inspectable.h"
#include "../../../common/Serializable.h"


namespace Metal {
    struct Camera final : Inspectable, Serializable {
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
        float fov = 90;
        float aspectRatio = 1;
        float orthographicProjectionSize = 50;

        float pitch = 0.f;
        float yaw = 0.f;

        float lastMouseX = 0;
        float lastMouseY = 0;
        float deltaX = 0;
        float deltaY = 0;

        const char *getTitle() override;

        const char *getIcon() override;

        void extractFrustumPlanes(glm::mat4x4 m);

        bool isSphereInsideFrustum(glm::vec3 center, float radius) const;

        void registerFields() override;

        explicit Camera(float pitch, float yaw, glm::vec3 position);

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["rotationSensitivity"] = rotationSensitivity;
            j["movementSensitivity"] = movementSensitivity;
            j["zoomSensitivity"] = zoomSensitivity;
            j["motionBlurEnabled"] = motionBlurEnabled;
            j["motionBlurVelocityScale"] = motionBlurVelocityScale;
            j["motionBlurMaxSamples"] = motionBlurMaxSamples;
            j["cameraMotionBlur"] = cameraMotionBlur;
            j["bloomEnabled"] = bloomEnabled;
            j["filmGrain"] = filmGrain;
            j["vignetteEnabled"] = vignetteEnabled;
            j["chromaticAberrationEnabled"] = chromaticAberrationEnabled;
            j["distortionEnabled"] = distortionEnabled;
            j["filmGrainStrength"] = filmGrainStrength;
            j["vignetteStrength"] = vignetteStrength;
            j["bloomThreshold"] = bloomThreshold;
            j["bloomQuality"] = bloomQuality;
            j["bloomOffset"] = bloomOffset;
            j["chromaticAberrationIntensity"] = chromaticAberrationIntensity;
            j["distortionIntensity"] = distortionIntensity;
            j["position"] = {position.x, position.y, position.z};
            j["isOrthographic"] = isOrthographic;
            j["zFar"] = zFar;
            j["zNear"] = zNear;
            j["fov"] = fov;
            j["aspectRatio"] = aspectRatio;
            j["orthographicProjectionSize"] = orthographicProjectionSize;
            j["pitch"] = pitch;
            j["yaw"] = yaw;
            j["lastMouseX"] = lastMouseX;
            j["lastMouseY"] = lastMouseY;
            j["deltaX"] = deltaX;
            j["deltaY"] = deltaY;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            rotationSensitivity = j.at("rotationSensitivity").get<float>();
            movementSensitivity = j.at("movementSensitivity").get<float>();
            zoomSensitivity = j.at("zoomSensitivity").get<float>();
            motionBlurEnabled = j.at("motionBlurEnabled").get<bool>();
            motionBlurVelocityScale = j.at("motionBlurVelocityScale").get<float>();
            motionBlurMaxSamples = j.at("motionBlurMaxSamples").get<int>();
            cameraMotionBlur = j.at("cameraMotionBlur").get<bool>();
            bloomEnabled = j.at("bloomEnabled").get<bool>();
            filmGrain = j.at("filmGrain").get<bool>();
            vignetteEnabled = j.at("vignetteEnabled").get<bool>();
            chromaticAberrationEnabled = j.at("chromaticAberrationEnabled").get<bool>();
            distortionEnabled = j.at("distortionEnabled").get<bool>();
            filmGrainStrength = j.at("filmGrainStrength").get<float>();
            vignetteStrength = j.at("vignetteStrength").get<float>();
            bloomThreshold = j.at("bloomThreshold").get<float>();
            bloomQuality = j.at("bloomQuality").get<int>();
            bloomOffset = j.at("bloomOffset").get<int>();
            chromaticAberrationIntensity = j.at("chromaticAberrationIntensity").get<float>();
            distortionIntensity = j.at("distortionIntensity").get<float>();
            position = {j.at("position")[0], j.at("position")[1], j.at("position")[2]};
            isOrthographic = j.at("isOrthographic").get<bool>();
            zFar = j.at("zFar").get<float>();
            zNear = j.at("zNear").get<float>();
            fov = j.at("fov").get<float>();
            aspectRatio = j.at("aspectRatio").get<float>();
            orthographicProjectionSize = j.at("orthographicProjectionSize").get<float>();
            pitch = j.at("pitch").get<float>();
            yaw = j.at("yaw").get<float>();
            lastMouseX = j.at("lastMouseX").get<float>();
            lastMouseY = j.at("lastMouseY").get<float>();
            deltaX = j.at("deltaX").get<float>();
            deltaY = j.at("deltaY").get<float>();
        }

    private:
        static void extractPlane(const glm::mat4 &matrix, int index, glm::vec4 &plane);

        glm::vec4 planes[6] = {glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}};
    };
}

#endif
