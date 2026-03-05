#ifndef ENGINEREPO_H
#define ENGINEREPO_H
#include "../../common/inspection/Inspectable.h"
#include <glm/vec3.hpp>
#include "../../util/Serializable.h"

namespace Metal {
    struct EngineRepository final : Inspectable, Serializable {
        bool vsync = true;

        bool isBaking = false;
        bool denoiserEnabled = false;
        bool dofEnabled = false;
        float dofFocusDistance = 10;
        float dofAperture = 1.2f;
        float dofFocalLength = 5;

        bool multipleImportanceSampling = false;
        int maxVideoFrames = 100;
        int shadingResInvScale = 2;
        int pathTracerMaxSamples = 200;
        float pathTracingEmissiveFactor = 2;
        int pathTracerSamples = 1;
        int pathTracerBounces = 1;
        int pathTracerAccumulationCount = 0;
        float pathTracerMultiplier = 1;
        int numberOfTiles = 10;
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
