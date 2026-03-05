#ifndef ENTITYASSETDATA_H
#define ENTITYASSETDATA_H
#include <string>

#include "../../util/Serializable.h"
#include <glm/glm.hpp>

namespace Metal {
    struct EntityAssetData final : Serializable {
        std::string name{};
        std::string meshId{};
        int parentEntity = -1;
        int id;

        std::string albedo;
        std::string roughness;
        std::string metallic;
        glm::vec3 albedoColor{1, 1, 1};
        float roughnessFactor = 1;
        float metallicFactor = 0;
        float transmissionFactor = 0;
        float thicknessFactor = 0;
        float ior = 1.45;
        bool isEmissive = false;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["id"] = id;
            j["name"] = name;
            j["meshId"] = meshId;
            j["parentEntity"] = parentEntity;

            j["albedo"] = albedo;
            j["roughness"] = roughness;
            j["metallic"] = metallic;
            j["albedoColor"] = {albedoColor.x, albedoColor.y, albedoColor.z};
            j["roughnessFactor"] = roughnessFactor;
            j["metallicFactor"] = metallicFactor;
            j["transmissionFactor"] = transmissionFactor;
            j["thicknessFactor"] = thicknessFactor;
            j["ior"] = ior;
            j["isEmissive"] = isEmissive;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            id = j.at("id").get<int>();
            name = j.at("name").get<std::string>();
            meshId = j.at("meshId").get<std::string>();
            parentEntity = j.at("parentEntity").get<int>();

            albedo = j.value("albedo", "");
            roughness = j.value("roughness", "");
            metallic = j.value("metallic", "");
            if (j.contains("albedoColor")) {
                albedoColor = {j.at("albedoColor")[0], j.at("albedoColor")[1], j.at("albedoColor")[2]};
            }
            roughnessFactor = j.value("roughnessFactor", 1.0f);
            metallicFactor = j.value("metallicFactor", 0.0f);
            transmissionFactor = j.value("transmissionFactor", 0.0f);
            thicknessFactor = j.value("thicknessFactor", 0.0f);
            ior = j.value("ior", 1.45f);
            isEmissive = j.value("isEmissive", false);
        }
    };
}
#endif //ENTITYASSETDATA_H
