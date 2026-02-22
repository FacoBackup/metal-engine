#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/glm.hpp>

namespace Metal {
    struct MeshComponent final : AbstractComponent, Serializable {
        std::string meshId;
        std::string materialId;

        int parallaxLayers = 16;
        unsigned int renderIndex = 0;
        float parallaxHeightScale = 1;

        void registerFields() override;

        void onUpdate(InspectableMember *member) override;

        ComponentTypes::ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //MESHCOMPONENT_H
