#include "PrimitiveComponent.h"
#include "../../ApplicationContext.h"
#include "../../editor/enum/EntryType.h"
#include "MeshData.h"

namespace Metal {
    void PrimitiveComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH);
        registerColor(albedoColor, "Material", "Albedo color");
        registerBool(isEmissive, "Material", "Is Emissive?");
        registerFloat(roughnessFactor, "Material", "Roughness factor", 0, 1);
        registerFloat(metallicFactor, "Material", "Metallic factor", 0, 1);
        registerFloat(transmissionFactor, "Material", "Transmission factor", 0, 1);
        registerFloat(thicknessFactor, "Material", "Thickness factor", 0, 10);
        registerFloat(ior, "Material", "IOR", 1, 3);
        registerResourceSelection(albedo, "Material", "Albedo", EntryType::TEXTURE);
        registerResourceSelection(roughness, "Material", "Roughness texture", EntryType::TEXTURE);
        registerResourceSelection(metallic, "Material", "Metallic Texture", EntryType::TEXTURE);
    }

    void PrimitiveComponent::onUpdate(InspectableMember *member) {
        // TODO - EVENT SYSTEM
        // if (member != nullptr && member->name == "meshId") {
        //     MeshData *data = CTX.meshService.loadMeshData(meshId);
        //     if (data != nullptr) {
        //         if (CTX.worldRepository.registry.all_of<TransformComponent>(entityId)) {
        //             CTX.worldRepository.registry.get<TransformComponent>(entityId).gizmoCenter = data->gizmoCenter;
        //         }
        //         delete data;
        //     }
        // }
        // CTX.engineContext.setGISettingsUpdated(true);
        // CTX.rayTracingService.setNeedsMaterialUpdate(true);
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
        j["isEmissive"] = isEmissive;
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
        isEmissive = j.at("isEmissive").get<bool>();

        albedoColor = {j.at("albedoColor")[0], j.at("albedoColor")[1], j.at("albedoColor")[2]};
    }
}
