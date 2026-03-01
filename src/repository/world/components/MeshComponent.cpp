#include "MeshComponent.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/EntryType.h"
#include "../../../service/mesh/MeshData.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH);
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL);
    }

    void MeshComponent::onUpdate(InspectableMember *member) {
        if (member != nullptr && member->name == "meshId") {
            MeshData *data = CTX.meshService.stream(meshId);
            if (data != nullptr) {
                const auto e = static_cast<entt::entity>(entityId);
                if (CTX.worldRepository.registry.all_of<TransformComponent>(e)) {
                    CTX.worldRepository.registry.get<TransformComponent>(e).gizmoCenter = data->gizmoCenter;
                }
                delete data;
            }
        }
        CTX.engineContext.setGISettingsUpdated(true);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }

    nlohmann::json MeshComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["meshId"] = meshId;
        j["materialId"] = materialId;
        return j;
    }

    void MeshComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<EntityID>();
        meshId = j.at("meshId").get<std::string>();
        materialId = j.at("materialId").get<std::string>();
    }
}
