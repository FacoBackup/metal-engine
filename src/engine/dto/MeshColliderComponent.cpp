#include "MeshColliderComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void MeshColliderComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", MESH_EXTENSIONS);
        registerBool(isConvex, "", "Is Convex");
    }

    ComponentType MeshColliderComponent::getType() {
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
