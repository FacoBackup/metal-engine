#include "MaterialFileData.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    void MaterialFileData::registerFields() {
        registerColor(albedoColor, "", "Albedo color");
        registerBool(isEmissive, "", "Is Emissive");
        registerFloat(roughnessFactor, "", "Roughness factor", 0, 1);
        registerFloat(metallicFactor, "", "Metallic factor", 0, 1);

        registerResourceSelection(albedo, "", "Albedo", EntryType::TEXTURE);
        registerResourceSelection(normal, "", "Normal", EntryType::TEXTURE);
        registerResourceSelection(ao, "", "Ambient occlusion", EntryType::TEXTURE);
        registerResourceSelection(roughness, "", "Roughness texture", EntryType::TEXTURE);
        registerResourceSelection(metallic, "", "Metallic Texture", EntryType::TEXTURE);
        registerResourceSelection(height, "", "Height Texture", EntryType::TEXTURE);
    }

    const char *MaterialFileData::getIcon() {
        return Icons::format_paint.c_str();
    }

    const char *MaterialFileData::getTitle() {
        return "Material";
    }

    nlohmann::json MaterialFileData::toJson() const {
        nlohmann::json j;
        j["albedo"] = albedo;
        j["normal"] = normal;
        j["roughness"] = roughness;
        j["metallic"] = metallic;
        j["height"] = height;
        j["albedoColor"] = {albedoColor.x, albedoColor.y, albedoColor.z};
        j["roughnessFactor"] = roughnessFactor;
        j["metallicFactor"] = metallicFactor;
        j["isEmissive"] = isEmissive;
        return j;
    }

    void MaterialFileData::fromJson(const nlohmann::json &j) {
        albedo = j.at("albedo").get<std::string>();
        normal = j.at("normal").get<std::string>();
        roughness = j.at("roughness").get<std::string>();
        metallic = j.at("metallic").get<std::string>();
        height = j.at("height").get<std::string>();
        roughnessFactor = j.at("roughnessFactor").get<float>();
        metallicFactor = j.at("metallicFactor").get<float>();
        isEmissive = j.at("isEmissive").get<bool>();

        albedoColor = {j.at("albedoColor")[0], j.at("albedoColor")[1], j.at("albedoColor")[2]};
    }
}
