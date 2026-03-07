#include "AtmosphereComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t) {
        return {
            (c1.x * (1 - t) + c2.x * t),
            (c1.y * (1 - t) + c2.y * t),
            (c1.z * (1 - t) + c2.z * t)
        };
    }

    glm::vec3 CalculateSunColor(const float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                           glm::vec3 &middayColor) {
        if (elevation <= -0.1f) {
            return nightColor;
        }
        if (elevation <= 0.0f) {
            float t = (elevation + 0.1f) / 0.1f;
            return BlendColors(nightColor, dawnColor, t);
        }
        if (elevation <= 0.5f) {
            float t = elevation / 0.5f;
            return BlendColors(dawnColor, middayColor, t);
        }
        return middayColor;
    }


    void AtmosphereComponent::registerFields() {
        registerBool(volumeEnabled, "Volume", "Enable volume scattering?");
        registerColor(albedo, "Volume", "Albedo");
        registerFloat(density, "Volume", "Density", .1, 10);
        registerFloat(g, "Volume", "Phase function asymmetry (0.0 for isotropic)", 0);
        registerFloat(scatteringAlbedo, "Volume", "Scattering albedo", 0);
        registerInt(samples, "Volume", "Samples", 1);
        registerInt(volumeShadowSteps, "Volume", "Shadow steps", 1);

        registerBool(atmosphereEnabled, "Atmosphere", "Enable atmosphere?");
        registerFloat(elapsedTime, "Atmosphere", "Elapsed time");
        registerFloat(sunDistance, "Atmosphere", "Sun distance");
        registerFloat(sunRadius, "Atmosphere", "Sun radius");
        registerFloat(sunLightIntensity, "Atmosphere", "Sun light intensity");
        registerColor(dawnColor, "Sun", "Dawn color");
        registerColor(nightColor, "Sun", "Night color");
        registerColor(middayColor, "Sun", "Midday color");
    }

    ComponentType AtmosphereComponent::getType() {
        return ComponentType::ATMOSPHERE;
    }

    void AtmosphereComponent::onUpdate(InspectableMember *member) {
        CTX.engineContext.setGISettingsUpdated(true);
        CTX.engineContext.setUpdateLights(true);

        sunPosition = glm::vec3(0,
                                   std::cos(elapsedTime),
                                   std::sin(elapsedTime)) * sunDistance;
        sunColor = CalculateSunColor(
                sunPosition.y / sunDistance,
                nightColor, dawnColor,
                middayColor);
    }

    nlohmann::json AtmosphereComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["albedo"] = {albedo.x, albedo.y, albedo.z};
        j["density"] = density;
        j["g"] = g;
        j["scatteringAlbedo"] = scatteringAlbedo;
        j["samples"] = samples;
        j["sunDistance"] = sunDistance;
        j["sunRadius"] = sunRadius;
        j["sunLightIntensity"] = sunLightIntensity;
        j["elapsedTime"] = elapsedTime;
        j["atmosphereEnabled"] = atmosphereEnabled;
        j["volumeShadowSteps"] = volumeShadowSteps;
        j["volumeEnabled"] = volumeEnabled;
        j["dawnColor"] = {dawnColor.x, dawnColor.y, dawnColor.z};
        j["nightColor"] = {nightColor.x, nightColor.y, nightColor.z};
        j["middayColor"] = {middayColor.x, middayColor.y, middayColor.z};
        j["volumeScale"] = {volumeScale.x, volumeScale.y, volumeScale.z};
        return j;
    }

    void AtmosphereComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        albedo = {j.at("albedo")[0], j.at("albedo")[1], j.at("albedo")[2]};
        density = j.at("density").get<float>();
        g = j.at("g").get<float>();
        scatteringAlbedo = j.at("scatteringAlbedo").get<float>();
        samples = j.at("samples").get<int>();
        volumeShadowSteps = j.at("volumeShadowSteps").get<int>();
        sunDistance = j.at("sunDistance").get<float>();
        sunRadius = j.at("sunRadius").get<float>();
        sunLightIntensity = j.at("sunLightIntensity").get<float>();
        elapsedTime = j.at("elapsedTime").get<float>();
        atmosphereEnabled = j.at("atmosphereEnabled").get<bool>();
        volumeEnabled = j.at("volumeEnabled").get<bool>();
        dawnColor = {j.at("dawnColor")[0], j.at("dawnColor")[1], j.at("dawnColor")[2]};
        nightColor = {j.at("nightColor")[0], j.at("nightColor")[1], j.at("nightColor")[2]};
        middayColor = {j.at("middayColor")[0], j.at("middayColor")[1], j.at("middayColor")[2]};
        volumeScale = {j.at("volumeScale")[0], j.at("volumeScale")[1], j.at("volumeScale")[2]};
    }
} // Metal
