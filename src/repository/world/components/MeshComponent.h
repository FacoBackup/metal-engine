#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct MeshComponent final : AbstractComponent {
        std::string meshId;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        SERIALIZE_TEMPLATE(meshId, entityId)
    };
}
#endif //MESHCOMPONENT_H
