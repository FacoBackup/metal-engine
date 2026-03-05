#include "EngineRepository.h"

#include "../../common/interface/Icons.h"
#include "../../context/ApplicationContext.h"

#define LEVEL_OF_DETAIL "Level of detail"
#define PATH_TRACER "Path tracer"
#define ATMOSPHERE "Atmosphere"
#define SUN "Sun"
#define DEPTH_OF_FIELD "Depth of field"

namespace Metal {
    void EngineRepository::registerFields() {
        registerInt(shadingResInvScale, "Display settings (Restart required)", "Shading inverted resolution scale", 1,
                    16);
        registerBool(vsync, "Display settings (Restart required)", "VSync?");
        registerInt(numberOfTiles, "World", "Number of tiles", 2, 100);
        // registerInt(maxVideoFrames, "", "Max video frames", 1);


        registerFloat(pathTracerMultiplier, PATH_TRACER, "Strength");
        registerBool(denoiserEnabled, PATH_TRACER, "Enable denoiser?");
        registerInt(pathTracerMaxSamples, PATH_TRACER, "Maximum accumulation", 1, 10000);
        registerInt(pathTracerSamples, PATH_TRACER, "Samples per pixel", 1, 32);
        registerInt(pathTracerBounces, PATH_TRACER, "Bounces", 0, 7);
        registerBool(multipleImportanceSampling, PATH_TRACER, "Enable multiple importance sampling?");
        registerFloat(pathTracingEmissiveFactor, PATH_TRACER, "Emissive surface factor", 0);

        registerBool(dofEnabled, DEPTH_OF_FIELD, "Enable depth of field?");
    }

    void EngineRepository::onUpdate(InspectableMember *member) {
        if (member != nullptr && member->name == LEVEL_OF_DETAIL) {
            CTX.worldGridRepository.hasMainTileChanged = true;
        }
        if (member != nullptr && (member->group == PATH_TRACER || member->group == DEPTH_OF_FIELD)) {
            CTX.engineContext.setGISettingsUpdated(true);
            CTX.engineContext.setUpdateLights(true);
        }
    }

    nlohmann::json EngineRepository::toJson() const {
        nlohmann::json j;
        j["vsync"] = vsync;
        j["pathTracerSamples"] = pathTracerSamples;
        j["multipleImportanceSampling"] = multipleImportanceSampling;
        j["pathTracerBounces"] = pathTracerBounces;
        j["svoFilePaths"] = svoFilePaths;
        j["pathTracingEmissiveFactor"] = pathTracingEmissiveFactor;
        j["pathTracerMultiplier"] = pathTracerMultiplier;
        j["shadingResInvScale"] = shadingResInvScale;
        j["pathTracerMaxSamples"] = pathTracerMaxSamples;
        j["denoiserEnabled"] = denoiserEnabled;
        j["dofEnabled"] = dofEnabled;
        j["dofFocusDistance"] = dofFocusDistance;
        j["dofAperture"] = dofAperture;
        j["dofFocalLength"] = dofFocalLength;
        return j;
    }

    void EngineRepository::fromJson(const nlohmann::json &j) {
        vsync = j.at("vsync").get<bool>();
        pathTracerSamples = j.at("pathTracerSamples").get<int>();
        multipleImportanceSampling = j.at("multipleImportanceSampling").get<bool>();
        pathTracerBounces = j.at("pathTracerBounces").get<int>();
        svoFilePaths = j.at("svoFilePaths").get<std::vector<std::string> >();
        pathTracingEmissiveFactor = j.at("pathTracingEmissiveFactor").get<float>();
        pathTracerMultiplier = j.at("pathTracerMultiplier").get<float>();
        shadingResInvScale = j.at("shadingResInvScale").get<int>();
        pathTracerMaxSamples = j.at("pathTracerMaxSamples").get<int>();
        denoiserEnabled = j.at("denoiserEnabled").get<bool>();
        dofEnabled = j.at("dofEnabled").get<bool>();
        dofFocusDistance = j.at("dofFocusDistance").get<float>();
        dofAperture = j.at("dofAperture").get<float>();
        dofFocalLength = j.at("dofFocalLength").get<float>();
    }

    const char *EngineRepository::getIcon() {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() {
        return "Engine settings";
    }
} // Metal
