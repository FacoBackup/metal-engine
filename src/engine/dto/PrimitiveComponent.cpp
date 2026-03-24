#include "PrimitiveComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"
#include "MeshData.h"

namespace Metal {
    void PrimitiveComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", MESH_EXTENSIONS);
        registerFloat(transmissionFactor, "", "Transmission factor", 0, 1);
        registerFloat(thicknessFactor, "", "Thickness factor", 0, 10);
        registerFloat(ior, "", "IOR", 1, 3);
        registerResourceSelection(albedo, "", "Albedo", TEXTURE_EXTENSIONS);
        registerResourceSelection(roughness, "", "Roughness texture", TEXTURE_EXTENSIONS);
        registerResourceSelection(metallic, "", "Metallic Texture", TEXTURE_EXTENSIONS);
    }

    ComponentType PrimitiveComponent::getType() const {
        return ComponentType::PRIMITIVE;
    }

    nlohmann::json PrimitiveComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["meshId"] = meshId;
        j["albedo"] = albedo;
        j["roughness"] = roughness;
        j["metallic"] = metallic;
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
        transmissionFactor = j.value("transmissionFactor", 0.0f);
        thicknessFactor = j.value("thicknessFactor", 0.0f);
        ior = j.value("ior", 1.45f);
    }
}
