#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"
#include <glm/glm.hpp>


namespace Metal {
    struct MeshComponent final : AbstractComponent {
        std::string meshId;
        std::string materialId;

        void registerFields() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        ComponentTypes::ComponentType getType() override;

        SERIALIZE_TEMPLATE(entityId, meshId, materialId)
    };
}
#endif //MESHCOMPONENT_H
