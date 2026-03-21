#include "PrimitiveComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"
#include "MeshData.h"

namespace Metal {
    void PrimitiveComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", MESH_EXTENSIONS);
        registerColor(albedoColor, "Material", "Albedo color");
        registerFloat(roughnessFactor, "Material", "Roughness factor", 0, 1);
        registerFloat(metallicFactor, "Material", "Metallic factor", 0, 1);
        registerFloat(transmissionFactor, "Material", "Transmission factor", 0, 1);
        registerFloat(thicknessFactor, "Material", "Thickness factor", 0, 10);
        registerFloat(ior, "Material", "IOR", 1, 3);
        registerResourceSelection(albedo, "Material", "Albedo", TEXTURE_EXTENSIONS);
        registerResourceSelection(roughness, "Material", "Roughness texture", TEXTURE_EXTENSIONS);
        registerResourceSelection(metallic, "Material", "Metallic Texture", TEXTURE_EXTENSIONS);
    }

    ComponentType PrimitiveComponent::getType() {
        return ComponentType::PRIMITIVE;
    }

    nlohmann::json PrimitiveComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["meshId"] = meshId;
        j["albedo"] = albedo;
        j["roughness"] = roughness;
        j["metallic"] = metallic;
        j["albedoColor"] = {albedoColor.x, albedoColor.y, albedoColor.z};
        j["roughnessFactor"] = roughnessFactor;
        j["metallicFactor"] = metallicFactor;
        j["transmissionFactor"] = transmissionFactor;
        j["thicknessFactor"] = thicknessFactor;
        j["ior"] = ior;
        return j;
    }

    void PrimitiveComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        meshId = j.at("meshId").get<std::string>();

        albedo = j.at("albedo").get<std::string>();
        roughness = j.at("roughness").get<std::string>();
        metallic = j.at("metallic").get<std::string>();
        roughnessFactor = j.at("roughnessFactor").get<float>();
        metallicFactor = j.at("metallicFactor").get<float>();
        transmissionFactor = j.value("transmissionFactor", 0.0f);
        thicknessFactor = j.value("thicknessFactor", 0.0f);
        ior = j.value("ior", 1.45f);

        albedoColor = {j.at("albedoColor")[0], j.at("albedoColor")[1], j.at("albedoColor")[2]};
    }
}
