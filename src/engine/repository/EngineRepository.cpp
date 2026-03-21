#include "EngineRepository.h"

#include "../../common/Icons.h"
#include "../../ApplicationContext.h"

#define PATH_TRACER "Path tracer"
#define ATMOSPHERE "Atmosphere"
#define SUN "Sun"
#define VOLUMES "Volumes"
#define DEPTH_OF_FIELD "Depth of field"
#define PHYSICS "Physics"

namespace Metal {
    void EngineRepository::registerFields() {
        registerInt(shadingResInvScale, "Display settings (Restart required)", "Shading inverted resolution scale", 1,
                    16);
        registerBool(vsync, "Display settings (Restart required)", "VSync?");

        registerInt(volumeShadowSteps, VOLUMES, "Shadow steps", 1);

        registerFloat(pathTracerMultiplier, PATH_TRACER, "Strength");
        registerBool(denoiserEnabled, PATH_TRACER, "Enable denoiser?");
        registerInt(pathTracerMaxSamples, PATH_TRACER, "Maximum accumulation", 1, 10000);
        registerInt(pathTracerSamples, PATH_TRACER, "Samples per pixel", 1, 32);
        registerInt(pathTracerBounces, PATH_TRACER, "Bounces", 0, 7);
        registerFloat(pathTracingEmissiveFactor, PATH_TRACER, "Emissive surface factor", 0);

        registerBool(dofEnabled, DEPTH_OF_FIELD, "Enable depth of field?");
        registerFloat(dofFocusDistance, DEPTH_OF_FIELD, "Focus distance");
        registerFloat(dofAperture, DEPTH_OF_FIELD, "Aperture");
        registerFloat(dofFocalLength, DEPTH_OF_FIELD, "Focal length");

        registerBool(atmosphereEnabled, ATMOSPHERE, "Enable atmosphere?");
        registerFloat(elapsedTime, ATMOSPHERE, "Elapsed time");

        registerVec3(gravity, PHYSICS, "Gravity");
        registerBool(physicsPaused, PHYSICS, "Paused?");
    }

    nlohmann::json EngineRepository::toJson() const {
        nlohmann::json j;
        j["vsync"] = vsync;
        j["pathTracerSamples"] = pathTracerSamples;
        j["pathTracerBounces"] = pathTracerBounces;
        j["atmosphereEnabled"] = atmosphereEnabled;
        j["elapsedTime"] = elapsedTime;
        j["pathTracingEmissiveFactor"] = pathTracingEmissiveFactor;
        j["pathTracerMultiplier"] = pathTracerMultiplier;
        j["shadingResInvScale"] = shadingResInvScale;
        j["pathTracerMaxSamples"] = pathTracerMaxSamples;
        j["volumeShadowSteps"] = volumeShadowSteps;
        j["denoiserEnabled"] = denoiserEnabled;
        j["dofEnabled"] = dofEnabled;
        j["dofFocusDistance"] = dofFocusDistance;
        j["dofAperture"] = dofAperture;
        j["dofFocalLength"] = dofFocalLength;
        j["gravity"] = {gravity.x, gravity.y, gravity.z};
        j["physicsPaused"] = physicsPaused;
        return j;
    }

    void EngineRepository::fromJson(const nlohmann::json &j) {
        vsync = j.at("vsync").get<bool>();
        pathTracerSamples = j.at("pathTracerSamples").get<int>();
        pathTracerBounces = j.at("pathTracerBounces").get<int>();
        atmosphereEnabled = j.at("atmosphereEnabled").get<bool>();
        elapsedTime = j.at("elapsedTime").get<float>();
        pathTracingEmissiveFactor = j.at("pathTracingEmissiveFactor").get<float>();
        pathTracerMultiplier = j.at("pathTracerMultiplier").get<float>();
        shadingResInvScale = j.at("shadingResInvScale").get<int>();
        pathTracerMaxSamples = j.at("pathTracerMaxSamples").get<int>();
        denoiserEnabled = j.at("denoiserEnabled").get<bool>();
        volumeShadowSteps = j.at("volumeShadowSteps").get<int>();
        dofEnabled = j.at("dofEnabled").get<bool>();
        dofFocusDistance = j.at("dofFocusDistance").get<float>();
        dofAperture = j.at("dofAperture").get<float>();
        dofFocalLength = j.at("dofFocalLength").get<float>();
        if (j.contains("gravity")) {
            gravity = {j.at("gravity")[0], j.at("gravity")[1], j.at("gravity")[2]};
        }
        physicsPaused = j.value("physicsPaused", false);
    }

    const char *EngineRepository::getIcon() {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() {
        return "Engine settings";
    }
} // Metal
