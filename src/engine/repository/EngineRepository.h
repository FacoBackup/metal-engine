#ifndef ENGINEREPO_H
#define ENGINEREPO_H
#include "../../../common/Inspectable.h"
#include <glm/vec3.hpp>
#include "../../../common/Serializable.h"

namespace Metal {
    struct EngineRepository final : Inspectable, Serializable {
        bool vsync = true;

        bool denoiserEnabled = false;
        bool dofEnabled = false;
        float dofFocusDistance = 10;
        float dofAperture = 1.2f;
        float dofFocalLength = 5;

        bool multipleImportanceSampling = false;
        int volumeShadowSteps = 8;
        int shadingResInvScale = 2;
        int pathTracerMaxSamples = 200;
        float pathTracingEmissiveFactor = 2;
        int pathTracerSamples = 1;
        int pathTracerBounces = 1;
        int pathTracerAccumulationCount = 0;
        float pathTracerMultiplier = 1;
        float elapsedTime = .5f;
        bool atmosphereEnabled = false;
        float sunDistance = 10000;
        float sunRadius = 3000;
        float sunLightIntensity = 2;
        glm::vec3 dawnColor{1, .39f, .19f};
        glm::vec3 nightColor{.1f, .1f, .1f};
        glm::vec3 middayColor{.9f, .9f, .9f};
        std::vector<std::string> svoFilePaths{};

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
} // Metal

#endif //ENGINEREPO_H
