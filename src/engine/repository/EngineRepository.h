#ifndef ENGINEREPO_H
#define ENGINEREPO_H
#include <glm/vec3.hpp>

#include "../../common/IRepository.h"

namespace Metal {
    struct EngineRepository final : IRepository {
        bool vsync = true;

        bool denoiserEnabled = false;
        bool dofEnabled = false;
        float dofFocusDistance = 10;
        float dofAperture = 1.2f;
        float dofFocalLength = 5;

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

        glm::vec3 gravity{0, -9.81f, 0};

        void registerFields() override;

        const char *getIcon() const override;

        const char *getTitle() const override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
} // Metal

#endif //ENGINEREPO_H
