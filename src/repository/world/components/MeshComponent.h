#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/glm.hpp>

namespace Metal {
    struct MeshComponent final : AbstractComponent, Serializable {
        std::string meshId;
        std::string materialId;

        unsigned int renderIndex = 0;

        void registerFields() override;

        void onUpdate(InspectableMember *member) override;

        ComponentTypes::ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //MESHCOMPONENT_H
