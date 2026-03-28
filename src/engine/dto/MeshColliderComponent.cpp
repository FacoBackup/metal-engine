#include "MeshColliderComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void MeshColliderComponent::registerFields() {
        static const std::vector<const FileExtensionInfo*> meshExtensions = {Metal::FileExtensions::mesh.get()};
        registerResourceSelection(meshId, "", "Mesh", meshExtensions);
    }

    ComponentType MeshColliderComponent::getType() const {
        return ComponentType::MESH_COLLIDER;
    }

    nlohmann::json MeshColliderComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["meshId"] = meshId;
        j["isConvex"] = isConvex;
        return j;
    }

    void MeshColliderComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        meshId = j.value("meshId", "");
        isConvex = j.value("isConvex", false);
    }
}
