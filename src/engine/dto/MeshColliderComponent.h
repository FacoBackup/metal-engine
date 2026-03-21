#ifndef MESH_COLLIDER_COMPONENT_H
#define MESH_COLLIDER_COMPONENT_H

#include <string>
#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct MeshColliderComponent final : AbstractComponent, ISerialize {
        std::string meshId;
        bool isConvex = false;

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //MESH_COLLIDER_COMPONENT_H
