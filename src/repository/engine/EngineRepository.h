#ifndef ENGINEREPO_H
#define ENGINEREPO_H
#include "../../common/inspection/Inspectable.h"
#include <glm/vec3.hpp>
#include "../../util/Serializable.h"

namespace Metal {
    struct EngineRepository final : Inspectable, Serializable {
        bool vsync = true;

        bool enabledDenoiser = true;
        bool isBaking = false;
        float denoiserDiffWeight = .5;

        bool multipleImportanceSampling = false;
        int maxVideoFrames = 100;
        int volumeShadowSteps = 8;
        int shadingResInvScale = 2;
        int pathTracerMaxSamples = 200;
        int giTileSubdivision = 40;
        float giEmissiveFactor = 2;
        int pathTracerSamples = 1;
        int pathTracerBounces = 1;
        int pathTracerAccumulationCount = 0;
        float pathTracerMultiplier = 1;
        int numberOfTiles = 10;
        float elapsedTime = .5f;
        bool atmosphereEnabled = false;
        bool incrementTime = false;
        float elapsedTimeSpeed = 1;
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

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["vsync"] = vsync;
            j["pathTracerSamples"] = pathTracerSamples;
            j["multipleImportanceSampling"] = multipleImportanceSampling;
            j["pathTracerBounces"] = pathTracerBounces;
            j["atmosphereEnabled"] = atmosphereEnabled;
            j["elapsedTime"] = elapsedTime;
            j["incrementTime"] = incrementTime;
            j["elapsedTimeSpeed"] = elapsedTimeSpeed;
            j["sunRadius"] = sunRadius;
            j["sunDistance"] = sunDistance;
            j["sunLightIntensity"] = sunLightIntensity;
            j["dawnColor"] = {dawnColor.x, dawnColor.y, dawnColor.z};
            j["nightColor"] = {nightColor.x, nightColor.y, nightColor.z};
            j["middayColor"] = {middayColor.x, middayColor.y, middayColor.z};
            j["svoFilePaths"] = svoFilePaths;
            j["giTileSubdivision"] = giTileSubdivision;
            j["giEmissiveFactor"] = giEmissiveFactor;
            j["pathTracerMultiplier"] = pathTracerMultiplier;
            j["shadingResInvScale"] = shadingResInvScale;
            j["pathTracerMaxSamples"] = pathTracerMaxSamples;
            j["denoiserDiffWeight"] = denoiserDiffWeight;
            j["enabledDenoiser"] = enabledDenoiser;
            j["volumeShadowSteps"] = volumeShadowSteps;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            vsync = j.at("vsync").get<bool>();
            pathTracerSamples = j.at("pathTracerSamples").get<int>();
            multipleImportanceSampling = j.at("multipleImportanceSampling").get<bool>();
            pathTracerBounces = j.at("pathTracerBounces").get<int>();
            atmosphereEnabled = j.at("atmosphereEnabled").get<bool>();
            elapsedTime = j.at("elapsedTime").get<float>();
            incrementTime = j.at("incrementTime").get<bool>();
            elapsedTimeSpeed = j.at("elapsedTimeSpeed").get<float>();
            sunRadius = j.at("sunRadius").get<float>();
            sunDistance = j.at("sunDistance").get<float>();
            sunLightIntensity = j.at("sunLightIntensity").get<float>();
            dawnColor = {j.at("dawnColor")[0], j.at("dawnColor")[1], j.at("dawnColor")[2]};
            nightColor = {j.at("nightColor")[0], j.at("nightColor")[1], j.at("nightColor")[2]};
            middayColor = {j.at("middayColor")[0], j.at("middayColor")[1], j.at("middayColor")[2]};
            svoFilePaths = j.at("svoFilePaths").get<std::vector<std::string>>();
            giTileSubdivision = j.at("giTileSubdivision").get<int>();
            giEmissiveFactor = j.at("giEmissiveFactor").get<float>();
            pathTracerMultiplier = j.at("pathTracerMultiplier").get<float>();
            shadingResInvScale = j.at("shadingResInvScale").get<int>();
            pathTracerMaxSamples = j.at("pathTracerMaxSamples").get<int>();
            denoiserDiffWeight = j.at("denoiserDiffWeight").get<float>();
            enabledDenoiser = j.at("enabledDenoiser").get<bool>();
            volumeShadowSteps = j.at("volumeShadowSteps").get<int>();
        }
    };
} // Metal

#endif //ENGINEREPO_H
